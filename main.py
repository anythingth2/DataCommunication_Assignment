import CameraReader
import ServoArduino
import Config
import cv2
import serial
import Util

try:
    CameraReader.initPort()
    print('CameraRead Initilized')
    # ServoArduino.initPort()
    print('ServoArduino Initilized')
except serial.SerialException:
    print('initilize port error')
    Util.getListComPort(verbose=True)
    exit()

while True:

    
    cv2.imshow('o7',CameraReader.readImage())
    amountDot = CameraReader.countDotOnLastImage()
    print('black dot = {0} ({1} percents of image)'.format(amountDot,amountDot/(240*320)*100))
    # servoCommand = ServoArduino.getCommand()
    # print('get command = {}'.format(servoCommand))
    # if servoCommand == Config.COMMAND.CAPTURE_AND_SEND_DOT:
    #     ServoArduino.servo_port.write( amountDot.to_bytes(2,'little'))
    #     print('byte : ',amountDot.to_bytes(2,'little'))
    cv2.waitKey(1)
cv2.destroyAllWindows()
