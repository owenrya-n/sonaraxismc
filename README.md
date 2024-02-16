Angular Positioning using IMU and Stepper Motors


1. TCP/IP: (preferred)

    Connect to Rpi over telnet at port 23
    Controller_telnet.py and venv activated automatically on Rpi startup
    Send Commands to Pi over Telnet
    Potentially use regex to parse at console


3. SSH:
   
    SSH into RPi and run Controller.py to access functions


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
