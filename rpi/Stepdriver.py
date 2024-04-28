import sys
sys.path.append('/home/axes/Desktop/Qwiic_9DoF_IMU_ICM20948_Py')
import RPi.GPIO as GPIO
import time

class Stepdriver:
    def __init__(self, step_pin, dir_pin, steps_per_revolution):
        self.step_pin = step_pin
        self.dir_pin = dir_pin
        self.steps_per_revolution = steps_per_revolution
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(self.step_pin, GPIO.OUT)
        GPIO.setup(self.dir_pin, GPIO.OUT)

    def use_profile(self, steps, direction, acceleration, max_speed):
        GPIO.output(self.dir_pin, direction)

        acceleration_steps = steps // 3
        constant_speed_steps = steps - 2 * acceleration_steps

        self.setrate(acceleration_steps, acceleration)

        self.setrate(constant_speed_steps, max_speed)

        self.setrate(acceleration_steps, acceleration, reverse=True)

    def move_angle(self, angle, direction, speed):
        steps = int(angle / 360 * self.steps_per_revolution)
        self.use_profile(steps, direction, acceleration=50, max_speed=speed)

    def setrate(self, steps, speed, reverse=False):
        delay = 1 / speed
        for _ in range(steps):
            GPIO.output(self.step_pin, GPIO.HIGH)
            time.sleep(delay)
            GPIO.output(self.step_pin, GPIO.LOW)
            time.sleep(delay)

    def cleanup(self):
        GPIO.cleanup()
