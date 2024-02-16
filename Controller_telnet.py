import RPi.GPIO as GPIO
import time
from Stepdriver import Stepdriver
from Imureader import Imureader
import socket

class Controller:
    def __init__(self):
        self.stepper_roll = Stepdriver(step_pin=11, dir_pin=13, steps_per_revolution=200)
        self.stepper_pitch = Stepdriver(step_pin=15, dir_pin=16, steps_per_revolution=200)
        self.imu = Imureader()
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind(('169.254.26.196', 23))
        self.server_socket.listen(1)
        print("Telnet Connection Successful")

    def set_axes(self, roll, pitch):
        d_roll = (roll - self.imu.get_corrected_roll())
        d_pitch = (pitch - self.imu.get_corrected_pitch())
        self.stepper_roll.move_angle(angle=d_roll, direction=GPIO.HIGH, speed=100)
        self.stepper_pitch.move_angle(angle=d_pitch, direction=GPIO.HIGH, speed=100)
        print("Roll set to " + str(roll) + " degrees, Pitch set to " + str(pitch) + " degrees")

    def get_axes(self):
        roll = self.imu.get_corrected_roll()
        pitch = self.imu.get_corrected_pitch()
        print("Roll: " + str(roll) + " degrees, Pitch: " + str(pitch) + " degrees")
        return roll, pitch

    def zero(self):
        self.imu.zero()
        print("IMU angles reset")

    def handle_telnet_connection(self, client_socket):
        client_socket.send("Welcome to the controller server!\n".encode())
        while True:
            data = client_socket.recv(1024).decode().strip()
            if not data:
                break
            elif data == "get_axes":
                roll, pitch = self.get_axes()
                client_socket.send(f"Roll: {roll} degrees, Pitch: {pitch} degrees\n".encode())
            elif data == "zero":
                self.zero()
                client_socket.send("IMU angles reset\n".encode())
            elif data.startswith("set_axes"):
                _, roll, pitch = data.split()
                self.set_axes(int(roll), int(pitch))
                client_socket.send("Axes set successfully\n".encode())
            else:
                client_socket.send("Invalid command\n".encode())
        client_socket.close()

    def start_telnet_server(self):
        try:
            while True:
                client_socket, _ = self.server_socket.accept()
                self.handle_telnet_connection(client_socket)
        except KeyboardInterrupt:
            print("Telnet server stopped")
        finally:
            self.server_socket.close()
            print("Telnet server socket closed")

    def cleanup(self):
        GPIO.cleanup()
        print("GPIO cleanup completed")

# Example usage:
if __name__ == "__main__":
    controller = Controller()
    try:
        controller.start_telnet_server()
    except Exception as e:
        print(f"Error: {e}")
    finally:
        controller.cleanup()
