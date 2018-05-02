class CAMERA_ARDUINO:
    PORT = 'COM6'
    BAUDRATE = 1000000
    width = 240
    height = 320
    BW_THRESOLD = 0.5

class SERVO_ARDUINO:
    PORT = 'COM17'
    BAUDRATE = 115200
class COMMAND:
    CAPTURE_AND_SEND_DOT = b'C'
    
    