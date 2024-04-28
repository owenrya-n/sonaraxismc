import RPi.GPIO as GPIO
import time
from Stepdriver import Stepdriver
from Imureader import Imureader

class Controller:
    def __init__(self):
        self.stepper_roll = Stepdriver(step_pin=11, dir_pin=13, steps_per_revolution=200)
        self.stepper_pitch = Stepdriver(step_pin=15, dir_pin=16, steps_per_revolution=200)
        self.imu = Imureader()

    def set_axes(self, roll, pitch):
        d_roll = (roll - imu.get_corrected_roll())
        d_pitch = (pitch - imu.get_corrected_pitch())
        self.stepper_roll.move_angle(angle=d_roll, direction=GPIO.HIGH, speed=100)
        self.stepper_pitch.move_angle(angle=d_pitch, direction=GPIO.HIGH, speed=100)
        print("Roll set to " + roll + " degrees, Pitch set to " + pitch + " degrees")

    def get_axes(self):
        imupitch = imu.get_pitch()
        imupitch = imu.get_roll()
        print("Roll: " + roll + " degrees, Pitch: " + pitch + " degrees")
        return imuroll, imupitch

    def reset(self)
        imu.zero()
        print("IMU angles reset")

    def cleanup(self):
        GPIO.cleanup()
        print("GPIO cleanup completed.")




