import serial
from Info import CAMERA_ARDUINO
from matplotlib import pyplot as plt
import numpy as np
import cv2
from Util import tic,toc


camPort = serial.Serial(port=CAMERA_ARDUINO.PORT,baudrate=CAMERA_ARDUINO.BAUDRATE)
img = np.ndarray((CAMERA_ARDUINO.height,CAMERA_ARDUINO.width),dtype='uint8')

def initPort():
    camPort = serial.Serial(port=info.CAMERA_ARDUINO.PORT,baudrate=info.CAMERA_ARDUINO.BAUDRATE)
def closePort():
    camPort.flushInput()
    camPort.flushOutput()
    camPort.close()
def flushSerial():
    camPort.flushInput()
    camPort.flushOutput()

def restartPort():
    try:
        closePort()
    except:
        pass
    initPort()

def waitingForImageReady():

    print('waiting for image ready')
    while True:
        if camPort.is_open:
            tmp = camPort.read()
            if tmp == b'*':
                tmp = camPort.read(4)
                if tmp == b'RDY*':
                    break
        else:
            raise serial.SerialException()
def readImage():
    tic()
    waitingForImageReady()
    print('Image is Ready! Start Reading Image from arduino')
    for x in range(CAMERA_ARDUINO.width):
        for y in range(CAMERA_ARDUINO.height-1,-1,-1):
            if camPort.is_open:
                img[y][x] = int.from_bytes(camPort.read(),byteorder='big')
            else:
                raise serial.SerialException()
    toc()
    print('Done, reading image.')
    print()
    return img

def countDotOnLastImage():
    count = 0
    thresold = CAMERA_ARDUINO.BW_THRESOLD * np.mean(img)
    for x in range(CAMERA_ARDUINO.width):
        for y in range(CAMERA_ARDUINO.height-1,-1,-1):
            if img[y][x] < thresold:
                count += 1
    return count
        