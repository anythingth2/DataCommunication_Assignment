import serial
import time
def getListComPort(verbose=False):
    available = []
    for i in range(256):
        try:
            portName = 'COM' + str(i)
            _ = serial.Serial(portName)
            available.append(portName)
            _.close()
        except serial.SerialException:
            pass
    if verbose:
        print('Found Ports :',' '.join(available))
    return available

time_logging = 0
def tic():
    global time_logging 
    time_logging = time.time()
def toc():
    print('Elapsed time ',time.time() - time_logging,'seconds')

