import serial
from Config import SERVO_ARDUINO

servo_port = serial.Serial()
def initPort():
    global servo_port
    servo_port = serial.Serial(port = SERVO_ARDUINO.PORT,baudrate= SERVO_ARDUINO.BAUDRATE)
    print('ServoArduinoPort = {} is open {}'.format(SERVO_ARDUINO.PORT,servo_port.is_open))
def getCommand():
    if servo_port.in_waiting >0:
        command = servo_port.read()
        return command
    else: return None