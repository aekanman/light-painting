Option Explicit On
Imports System.Math

Public Class Test_NumReg2
    ' Sample VB.NET program to display R[1] and send a new value
    ' to/from any controller
    '
    ' Declarations
    '

    '   Public Class NumRegTest

    Private mobjRobot As FRRobot.FRCRobot
    Private WithEvents mobjRegs As FRRobot.FRCVars
    Dim Test1 As FRRobot.FRCVars
    Dim fileReader As System.IO.StreamReader 

    ' Handle the Connect/Disconnect button click 
    Private Sub cmdConnect_Click(ByVal sender As Object, _
                                                             ByVal e As System.EventArgs) Handles cmdConnect.Click
        Try

            If cmdConnect.Text = "Connect" Then

                txtRegValue.Text = String.Format("Connecting to {0} Please wait.", txtHostName.Text)

                mobjRobot = New FRRobot.FRCRobot
                mobjRobot.Connect(txtHostName.Text)
                mobjRegs = mobjRobot.RegNumerics

                '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                'MAIN LOOP'
                '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

                '' move to Ready
                ''If mobjRobot.RegNumerics(2).value.reglong = 1 Then

                ''mobjRobot.RegNumerics(1).value.reglong = 2
                ''mobjRobot.RegNumerics(2).value.reglong = 0

                ''While mobjRobot.RegNumerics(2).value.reglong = 0
                ''System.Threading.Thread.Sleep(200)
                ''End While
                ''End If
                fileReader = My.Computer.FileSystem.OpenTextFileReader("C:\Users\Admin\Desktop\G11\160123_IK\MoveWithReg_4\path.txt")
                Dim stringReader As String
                Do Until (fileReader.EndOfStream)

                    stringReader = fileReader.ReadLine()

                    Dim parts As String() = stringReader.Split(New Char(), ","c)
                    Dim x1 As Short = Convert.ToDouble(parts(0))
                    Dim y1 As Short = Convert.ToDouble(parts(1))
                    Dim z1 As Short = Convert.ToDouble(parts(2))

                    '' MOVE TO JOINT COORDINATES
                    If mobjRobot.RegNumerics(2).Value.RegLong = 1 Then

                        Dim IKxyz(3) As Double
                        IKxyz = invKin(x1, y1, z1, 45)

                        '' theta1, theta2, theta3, theta4, theta5, speed (percent)
                        moveJnt(IKxyz(1), IKxyz(2), IKxyz(3), IKxyz(4), IKxyz(5), 20)
                        ''moveJnt(15, 30, 0, 0, 0, 20)

                        mobjRobot.RegNumerics(1).Value.RegLong = 4
                        mobjRobot.RegNumerics(2).Value.RegLong = 0
                        While mobjRobot.RegNumerics(2).Value.RegLong = 0
                            System.Threading.Thread.Sleep(200)
                        End While
                    End If

                Loop


                '' move to Home
                If mobjRobot.RegNumerics(2).value.reglong = 1 Then

                    mobjRobot.RegNumerics(1).value.reglong = 1
                    mobjRobot.RegNumerics(2).value.reglong = 0

                    While mobjRobot.RegNumerics(2).value.reglong = 0
                        System.Threading.Thread.Sleep(200)
                    End While
                End If


                '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                ''End the motion by setting R(1) to 99
                mobjRobot.RegNumerics(1).Value.RegLong = 99
                mobjRobot.RegNumerics(2).Value.RegLong = 0
                System.Threading.Thread.Sleep(2000)

                ''Zero the Register Values before disconnecting!
                zeroRegs()

            Else    ' must be the user wants to disconnect

                txtRegValue.Text = "Releasing the Robot objects"
                ReleaseObjects()
                txtRegValue.Text = "Not Connected"
            End If

        Catch ex As System.Runtime.InteropServices.COMException
            ' The only time an error is expected is during connect
            MsgBox(String.Format("{0} - {1}", ex.ErrorCode, ex.Message))
            ReleaseObjects()
        Catch ex As Exception
            MsgBox(ex.Message)
            ReleaseObjects()
        End Try

        If mobjRobot IsNot Nothing AndAlso mobjRobot.IsConnected Then
            cmdConnect.Text = "Disconnect"
        Else
            cmdConnect.Text = "Connect"
        End If

    End Sub

    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    '' FORWARD KINEMATICS
    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    Public Function invKin(ByVal x As Double, ByVal y As Double, ByVal z As Double, ByVal alpha As Double)

        '' Define pi:
        Dim pi, link2, link3, th1, th2, th3, th4, th5, th1f, th2f, th3f, th4f, th5f As Double
        pi = 22 / 7
        link2 = 260
        link3 = Sqrt(290 * 290 + 20 * 20)


        '' DH to Fanuc Conversion:
        Dim s, r, D, phi As Double

        th1 = Atan2(y, x)
        s = z
        r = Sqrt(x ^ 2 + y ^ 2)
        D = ((s ^ 2 + r ^ 2 - (link2 ^ 2) - (link3 ^ 2))) / (2 * (link2) * (link3))
        th3 = Atan2(-Sqrt(1 - D ^ 2), D)
        phi = Atan2(link3 * Sin(th3), link2 + link3 * Cos(th3))
        th2 = Atan2(s, r) - phi
        th4 = (pi / 2) - th2 - th3
        th5 = alpha - th1

        s = z
        r = Sqrt(Pow(x, 2) + Pow(y, 2))
        D = (Pow(s, 2) + Pow(r, 2) - Pow(link2, 2) - Pow(link3, 2)) / (2 * link2 * link3)

        th1 = Atan2(y, x)
        th3 = Atan2(-Sqrt(1 - Pow(D, 2)), D)
        th2 = Atan2(s, r) - Atan2(link3 * Sin(th3), link2 + link3 * Cos(th3))

        th1f = th1
        th2f = (pi / 2) - th2
        th3f = th2 + th3 - (4 * pi / 180)
        th4f = 0
        th5f = alpha * pi / 180 - th1f

        '' Radian to Degree Conversion:
        th1f = th1f * 180 / pi
        th2f = th2f * 180 / pi
        th3f = th3f * 180 / pi
        th4f = th4f * 180 / pi
        th5f = th5f * 180 / pi

        Dim outIK(5) As Double
        outIK(1) = th1f
        outIK(2) = th2f
        outIK(3) = th3f
        outIK(4) = th4f
        outIK(5) = th5f

        Return outIK

    End Function

    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    '' MOTION FUNCTIONS
    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    '' LINEAR MOTION FUNCTION
    Private Sub moveLin(xLin As Double, yLin As Double, zLin As Double, wLin As Double, pLin As Double, rLin As Double, speedLin As Double)
        '' POINT1
        mobjRobot.RegNumerics(11).Value.RegLong = xLin     '' Point1 x-axis
        mobjRobot.RegNumerics(12).Value.RegLong = yLin     '' Point1 y-axis
        mobjRobot.RegNumerics(13).Value.RegLong = zLin     '' Point1 z-axis
        mobjRobot.RegNumerics(14).Value.RegLong = wLin     '' Point1 w-aangle
        mobjRobot.RegNumerics(15).Value.RegLong = pLin     '' Point1 p-angle
        mobjRobot.RegNumerics(16).Value.RegLong = rLin     '' Point1 r-angle
        '%%%%%%%%
        mobjRobot.RegNumerics(10).Value.RegLong = speedLin

    End Sub


    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    '' JOINT MOTION FUNCTION
    Private Sub moveJnt(theta1 As Double, theta2 As Double, theta3 As Double, theta4 As Double, theta5 As Double, speedJnt As Double)
        '' POINT1
        mobjRobot.RegNumerics(21).Value.RegLong = theta1     '' Point1 x-axis
        mobjRobot.RegNumerics(22).Value.RegLong = theta2     '' Point1 y-axis
        mobjRobot.RegNumerics(23).Value.RegLong = theta3     '' Point1 z-axis
        mobjRobot.RegNumerics(24).Value.RegLong = theta4     '' Point1 w-aangle
        mobjRobot.RegNumerics(25).Value.RegLong = theta5     '' Point1 p-angle
        '%%%%%%%%
        mobjRobot.RegNumerics(20).Value.RegLong = speedJnt

    End Sub

    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    '' Zero the Register Values
    Private Sub zeroRegs()
        Dim counterZR As Integer = 1
        For counterZR = 1 To 200
            mobjRobot.RegNumerics(counterZR).Value.RegLong = 0
        Next counterZR
    End Sub

    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    ' Fully releasing COM objects requires waiting for garbage collection
    Private Sub ReleaseObjects()
        mobjRegs = ReleaseObject("mobjRegs", mobjRegs)
        mobjRobot = ReleaseObject("mobjRobot", mobjRobot)
        System.GC.Collect()
    End Sub

    ' Wrap object release in Try-Catch for enhanced diagnostics
    Private Function ReleaseObject(ByVal identifier As String, ByRef item As Object) As Object
        Try
            item = Nothing
        Catch ex As Exception
            System.Diagnostics.Trace.WriteLine(String.Format("Error releasing {0}.{1}Error: {2}", identifier, Environment.NewLine, ex.Message))
        End Try

        Return Nothing
    End Function

    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        '
        mobjRobot.RegNumerics(1).Value.RegLong = txtRegValue.Text
    End Sub

    Private Sub Test_NumReg2_Load(sender As Object, e As EventArgs) Handles MyBase.Load

    End Sub
End Class


