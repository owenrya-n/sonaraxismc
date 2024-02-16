from __future__ import print_function
import sys
sys.path.append('/home/axes/Desktop/Qwiic_9DoF_IMU_ICM20948_Py')
import qwiic_icm20948
import time
import sys

class Imureader:
    def __init__(self):
        self.IMU = qwiic_icm20948.QwiicIcm20948()
        if self.IMU.connected == False:
            print("IMU Connection Failure", \
                file=sys.stderr)
            return
        self.IMU.begin()
        self.zero()
        print("IMU Connection Success")

    def get_roll(self):
        if self.IMU.dataReady():
            self.IMU.getAgmt()
            return self.IMU.gxRaw
        else:
            print("Waiting for data")
            return None

    def get_pitch(self):
        if self.IMU.dataReady():
            self.IMU.getAgmt()
            return self.IMU.gyRaw
        else:
            print("Waiting for data")
            return None

    def zero(self):
        self.initial_roll = self.get_roll() if self.IMU.connected else 0
        self.initial_pitch = self.get_pitch() if self.IMU.connected else 0

    def get_corrected_roll(self):
        current_roll = self.get_roll()
        if current_roll is not None:
            return current_roll - self.initial_roll
        else:
            return None

    def get_corrected_pitch(self):
        current_pitch = self.get_pitch()
        if current_pitch is not None:
            return current_pitch - self.initial_pitch
        else:
            return None