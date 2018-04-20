import CameraReader
import cv2

while True:
    cv2.imshow('o7',CameraReader.readImage())
    amountDot = CameraReader.countDotOnLastImage()
    print('black dot = {0} ({1} percents of image)'.format(amountDot,amountDot/(240*320)*100))
    
    cv2.waitKey(1)
cv2.destroyAllWindows()
