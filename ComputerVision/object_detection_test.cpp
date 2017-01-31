// object_detection_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int VideoStreamColourDetection();
int StaticPhotoColourDetection();
int StaticPhotoEdgeDetection();
int VideoStreamEdgeDetection();
int StaticPhotoCircleDetection();
int VideoStreamColourTracking();
int VideoStreamColourTrackingNewControl();
int SampleCircleDetection();
int StaticPhotoContourDetection();

void wait(double seconds);

int main(int argc, char** argv)
{
	return StaticPhotoContourDetection();
	return 0;
}

int VideoStreamColourTracking()
{
	VideoCapture cap(1); //capture the video from webcam

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	int iLowH = 147;
	int iHighH = 179;

	int iLowS = 130;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	int iLastX = -1;
	int iLastY = -1;

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;


	bool draw = false;

	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		Mat imgHSV;

		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		Mat imgThresholded;

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
																						  //morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (removes small holes from the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 1000000)
		{
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;

			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
			{
				if (draw)
				{
					//Draw a red line from the previous point to the current point
					line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(255, 255, 255), 5);
				}
			}

			iLastX = posX;
			iLastY = posY;
		}

		//imgThresholded = imgThresholded + imgLines;
		imshow("Thresholded Image", imgThresholded); //show the thresholded image


		imgOriginal = imgOriginal - imgLines;
		imshow("Original", imgOriginal); //show the original image

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
		if (waitKey(10) == 32) //wait for 'space' key press for 30ms. If 'space' toggle drawing action
		{
			draw = !draw;
			cout << "drawing toggled" << endl;
			cout << draw << endl;
		}

		if (waitKey(10) == 99) //wait for 'C' key press for 30ms. If 'C' is pressed, clear the drawn line
		{
			imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;
			cout << "cleared" << endl;
		}
	}

	return 0;
}

int VideoStreamColourTrackingNewControl()
{
	VideoCapture cap(1); //capture the video from webcam

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	cvNamedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
	cvNamedWindow("Drawing Control", CV_WINDOW_AUTOSIZE);
	int iLowH = 147;
	int iHighH = 179;

	int iLowS = 130;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	int draw = 0;
	int clearScreen = 0;

	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	createTrackbar("HighV", "Control", &iHighV, 255);
	createTrackbar("HighV", "Control", &iHighV, 255);

	createTrackbar("Draw", "Drawing Control", &draw, 1);
	createTrackbar("Clear", "Drawing Control", &clearScreen, 1);

	int iLastX = -1;
	int iLastY = -1;

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;


	bool drawLines = false;
	int numPoints = 0;
	int numFrames = 0;

	vector<pair<double, double>> waypoints;

	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		Mat imgHSV;

		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		Mat imgThresholded;

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

																									  //morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (removes small holes from the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		// if the area <= 100000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 1000000)
		{
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;
			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
			{
				if (draw != 0)
				{
					numPoints++;
					//Draw a line from the previous point to the current point
					line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(255, 255, 255), 5);
					waypoints.push_back(pair<double, double>((double)posX / imgOriginal.size().width, (double)posY / imgOriginal.size().height));
				}
			}
			iLastX = posX;
			iLastY = posY;
		}

		imshow("Thresholded Image", imgThresholded); //show the thresholded image

		imgOriginal = imgOriginal - imgLines;
		imshow("Original", imgOriginal); //show the original image

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
		if (clearScreen != 0)
		{
			imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;
			cout << "Number of Path Points: " << numPoints << endl;
			clearScreen = 0;
			waypoints.clear();
			numPoints = 0;
		}
		cout << numFrames << endl;
		numFrames++;

		wait(0.5);
	}

	ofstream waypointsFile("waypoints.txt");
	if (waypointsFile.is_open())
	{
		for (int i = 0; i < waypoints.size(); i++)
		{
			waypointsFile << waypoints[i].first << "," << waypoints[i].second << endl;
		}
		waypointsFile.close();
	}

	return 0;
}
int VideoStreamColourDetection()
{
	VideoCapture cap(1); //capture the video from web cam
	//VideoCapture cap2(0);
	if (!cap.isOpened())// || !cap2.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars in "Control" window
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		Mat imgHSV;
		Mat imgThresholded;
		Mat imgMasked;
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

		//morphological opening (remove small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (fill small holes in the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		bitwise_and(imgOriginal, imgOriginal, imgMasked, imgThresholded);

		imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Original", imgOriginal); //show the original image		
		imshow("Masked Image", imgMasked);// show the masked image

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}

int StaticPhotoColourDetection()
{
	clock_t starttime = clock();

	cout << starttime << endl;

	Mat img = imread("C:/Users/Ahmed/Documents/University of Waterloo/FYDP/Test Photos/resized.jpg", CV_LOAD_IMAGE_UNCHANGED);
	Mat imgOriginal = img;
	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 65;

	//Create trackbars in "Control" window
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	Mat imgHSV;
	Mat imgThresholded;

	while (true)
	{
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV		

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
																						  //morphological opening (remove small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (fill small holes in the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		Mat imgFlooded = imgThresholded.clone();
		floodFill(imgFlooded, cv::Point(0, 0), Scalar(255));

		// Invert floodfilled image
		Mat imgFloodedInv = imgFlooded.clone();
		bitwise_not(imgFlooded, imgFloodedInv);

		//Find center
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		int posX = 0;
		int posY = 0;

		if (dArea > 10000)
		{
			//calculate the center of the deteceted object
			posX = dM10 / dArea;
			posY = dM01 / dArea;
		}

		Mat imgOrigianlWithcenter = imgOriginal.clone();
		circle(imgOrigianlWithcenter, Point(posX, posY), 5, Scalar(0, 0, 255), 5);

		imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Flooded", imgFlooded);
		imshow("Inverted", imgFloodedInv);
		imshow("Detected Center", imgOrigianlWithcenter);
		//imshow("Original", imgOriginal); //show the original image

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			imwrite("C:/Users/Ahmed/Documents/University of Waterloo/FYDP/Test Photos/temp/flooded_and_inverted.bmp", imgFloodedInv);
			break;
		}
	}
	return 0;
}


Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

/**
* @function CannyThreshold
* @brief Trackbar callback - Canny thresholds input with a ratio 1:3
*/
void CannyThreshold(int, void*)
{
	/// Reduce noise with a kernel 3x3
	blur(src_gray, detected_edges, Size(3, 3));

	/// Canny detector
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

	/// Using Canny's output as a mask, we display our result
	dst = Scalar::all(0);

	src.copyTo(dst, detected_edges);
	imshow(window_name, dst);
}

int StaticPhotoEdgeDetection() {

	while (true)
	{
		/// Load an image
		src = imread("C:/Users/Ahmed/Documents/University of Waterloo/FYDP/Test Photos/resized.jpg", CV_LOAD_IMAGE_UNCHANGED);//resized.jpg//dejan-1.jpg
		Mat imgOriginal = src;

		if (!src.data)
		{
			return -1;
		}

		/// Create a matrix of the same type and size as src (for dst)
		dst.create(src.size(), src.type());

		/// Convert the image to grayscale
		cvtColor(src, src_gray, CV_BGR2GRAY);

		/// Create a window
		namedWindow(window_name, CV_WINDOW_AUTOSIZE);

		/// Create a Trackbar for user to enter threshold
		createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

		/// Show the image
		//CannyThreshold(0, 0);
		/// Reduce noise with a kernel 3x3
		blur(src_gray, detected_edges, Size(3, 3));

		/// Canny detector
		Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

		/// Using Canny's output as a mask, we display our result
		dst = Scalar::all(0);

		src.copyTo(dst, detected_edges);
		imshow(window_name, dst);

		/// Wait until user exit program by pressing a key
		waitKey(0);
	}

	return 0;
}

int VideoStreamEdgeDetection() {

	/// Load an image
	VideoCapture cap(0); //capture the video from web cam

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}


	while (true)
	{
		bool bSuccess = cap.read(src); // read a new frame from video
									   //= imread("C:/Users/Ahmed/Documents/University of Waterloo/FYDP/Test Photos/resized.jpg", CV_LOAD_IMAGE_UNCHANGED);
		Mat imgOriginal = src;

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		if (!src.data)
		{
			return -1;
		}

		/// Create a matrix of the same type and size as src (for dst)
		dst.create(src.size(), src.type());

		/// Convert the image to grayscale
		cvtColor(src, src_gray, CV_BGR2GRAY);

		/// Create a window
		namedWindow(window_name, CV_WINDOW_AUTOSIZE);

		/// Create a Trackbar for user to enter threshold
		createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

		/// Show the image
		//CannyThreshold(0, 0);
		/// Reduce noise with a kernel 3x3
		blur(src_gray, detected_edges, Size(3, 3));

		/// Canny detector
		Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

		/// Using Canny's output as a mask, we display our result
		dst = Scalar::all(0);

		src.copyTo(dst, detected_edges);
		imshow(window_name, dst);

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
		/// Wait until user exit program by pressing a key
		//waitKey(0);
	}
	return 0;
}


int StaticPhotoCircleDetection()
{
	Mat img = imread("C:/Users/Ahmed/Documents/University of Waterloo/FYDP/Test Photos/board.jpg", CV_LOAD_IMAGE_UNCHANGED);
	Mat imgOriginal = img;
	Mat imgThresholded;
	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars in "Control" window
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	Mat src, src_gray;
	src = imgOriginal;
	resize(src, src, Size(640, 480));

	//src_gray = src;
	if (!src.data)
	{
		return -1;
	}

	/// Convert it to gray
	cvtColor(src, src_gray, CV_BGR2GRAY);

	/// Reduce the noise so we avoid false circle detection
	GaussianBlur(src_gray, src_gray, Size(9, 9), 2, 2);

	vector<Vec3f> circles;

		/// Apply the Hough Transform to find the circles
	HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT, 1, 30, 200, 50, 0, 0); //(src_gray, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows / 2, 200, 100, 50, 0);

	/// Draw the circles detected
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// circle outline
		circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
	}

	/// Show your results
	namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
	imshow("Hough Circle Transform Demo", src);
	imshow("Original", imgOriginal); //show the original image
	waitKey(0);
	return 0;

}

int SampleCircleDetection()
{
	Mat src, gray;
	src = imread("C:/Users/Ahmed/Documents/University of Waterloo/FYDP/Test Photos/board.jpg", CV_LOAD_IMAGE_COLOR);
	resize(src, src, Size(640, 480));
	cvtColor(src, gray, CV_BGR2GRAY);

	// Reduce the noise so we avoid false circle detection
	GaussianBlur(gray, gray, Size(9, 9), 2, 2);

	vector<Vec3f> circles;

	//circles.capacity.max_size = 10;

	// Apply the Hough Transform to find the circles
	HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 30, 200, 50, 0, 0);

	// Draw the circles detected
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center     
		circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);// circle outline
		cout << "center : " << center << "\nradius : " << radius << endl;
	}

	// Show your results
	namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
	imshow("Hough Circle Transform Demo", src);

	waitKey(0);
	return 0;
}

int StaticPhotoContourDetection()
{

	Mat image;
	image = imread("../temp/flooded_and_inverted.bmp", CV_LOAD_IMAGE_UNCHANGED);
	namedWindow("Display window", CV_WINDOW_AUTOSIZE);
	imshow("Display window", image);
	Mat gray;
	threshold(image, gray, 127, 255, 0);
	//cvtColor(image, gray, CV_BGR2GRAY);
	//Canny(gray, gray, 100, 200, 3);

	/// Find contours   
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345);
	waitKey(0);

	findContours(gray, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	/// Draw contours
	Mat drawing = Mat::zeros(gray.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
	}

	//imshow("Result window", drawing);
	waitKey(0);
	return 0;
}

void wait(double seconds)
{
	clock_t endwait;
	endwait = clock() + seconds * CLOCKS_PER_SEC;
	while (clock() < endwait) {}
}