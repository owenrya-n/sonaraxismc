from __future__ import print_function
import sys
sys.path.append('/home/axes/Desktop/Qwiic_9DoF_IMU_ICM20948_Py')
import qwiic_icm20948
import time
import sys
import threading
import math

class Imureader:
    def __init__(self):
        self.imu_sf = 1
        self.imu_res = 131
        self.num_samples = 100  
        self.IMU = qwiic_icm20948.QwiicIcm20948()
        if not self.IMU.connected:
            print("IMU Connection Failure", file=sys.stderr)
            return
        self.IMU.begin()
        self.roll = 0.0  
        self.pitch = 0.0  
        self.zero_triggered = False  
        print("IMU Connection Success")
        self.update_thread_stop = threading.Event()
        self.update_thread = threading.Thread(target=self._update_angles)
        self.update_thread.daemon = True
        self.update_thread.start()
        
        self.alpha = 0.003
        self.accel_roll_offset = 0.0
        self.accel_pitch_offset = 0.0

    def _update_angles(self):
        prev_time = time.time()
        while not self.update_thread_stop.is_set():
            if self.IMU.dataReady():
                curr_time = time.time()
                self.IMU.getAgmt()
                elapsed_time = curr_time - prev_time
                prev_time = curr_time

                delta_roll = self.IMU.gxRaw * elapsed_time / self.imu_res
                delta_pitch = self.IMU.gyRaw * elapsed_time / self.imu_res

                if self.zero_triggered:
                    sum_accel_roll = 0.0
                    sum_accel_pitch = 0.0
                    for _ in range(self.num_samples):
                        accel_x, accel_y, accel_z = self.IMU.axRaw / 1000, self.IMU.ayRaw / 1000, self.IMU.azRaw / 1000
                        sum_accel_roll += math.atan2(accel_y, accel_z) * (180.0 / math.pi)
                        sum_accel_pitch += math.atan2(-accel_x, math.sqrt(accel_y * accel_y + accel_z * accel_z)) * (
                                    180.0 / math.pi)
                        time.sleep(0.01)  
                    
                    self.accel_roll_offset = sum_accel_roll / self.num_samples
                    self.accel_pitch_offset = sum_accel_pitch / self.num_samples
                    
                    self.zero_triggered = False

                accel_x, accel_y, accel_z = self.IMU.axRaw / 1000, self.IMU.ayRaw / 1000, self.IMU.azRaw / 1000

                self.accel_roll = math.atan2(accel_y, accel_z) * (180.0 / math.pi) - self.accel_roll_offset
                self.accel_pitch = math.atan2(-accel_x, math.sqrt(accel_y * accel_y + accel_z * accel_z)) * (
                            180.0 / math.pi) - self.accel_pitch_offset

                self.roll = self.alpha * (self.roll + delta_roll) + (1 - self.alpha) * self.accel_roll
                self.pitch = self.alpha * (self.pitch + delta_pitch) + (1 - self.alpha) * self.accel_pitch

            time.sleep(0.001)

    def get_corrected_roll(self):
        return self.roll / self.imu_sf

    def get_corrected_pitch(self):
        return self.pitch / self.imu_sf

    def zero(self):
        self.zero_triggered = True
        self.roll = 0.0
        self.pitch = 0.0

    def get_roll(self):
        return self.roll

    def get_pitch(self):
        return self.pitch

    def kill(self):
        self.update_thread_stop.set()
        self.update_thread.join()
        if self.IMU.connected:
            self.IMU.end()
