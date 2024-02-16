Angular Positioning using IMU and Stepper Motors


A. TCP/IP: (preferred)

    1. Connect to Rpi over telnet at port 23
    2. Controller_telnet.py and venv start automatically (wait)
    3. Send Commands to Pi over Telnet
    4. Potentially use regex to parse at console


B. SSH:
   
    1. SSH into RPi and run Controller.py to access functions


Commands:

    set_axes(roll, pitch)
    #Positions platform at desired roll, pitch angles (deg)

    get_axes()
    #Returns current roll, pitch angles (deg)

    zero()
    #Defines the current angle of the platform as reference



Required Installs and Dependencies:

1. https://github.com/sparkfun/Qwiic_9DoF_IMU_ICM20948_Py

2. telnetd
