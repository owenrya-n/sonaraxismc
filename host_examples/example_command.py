import telnetlib
import time 

tn = telnetlib.Telnet("192.168.2.177", 23) #Opens telnet connection with host.

tn.write(b"M3,100;") #Move Motor three -1000 degrees
tn.write(b"\n\r") #Splitting the \n\r into a separate write is necessary here so that read_until recognizes the correct output

print(tn.read_until(b";")) #Read incoming message-- All new messages from the host and client are now terminated with a ;
#This should return ACK but returns b'\r\nACK' due to read_until binary
#I'll try to find workarounds and clean up the host side, but currently we may want to just use a regex



tn.write(b"?1") #Query value of axis 1
tn.write(b"\n\r") #Same as before
print(tn.read_until(b";")) #read until line ending
#same comments apply here as before 


tn.write(b"M3,-10000;")
tn.write(b"\n\r")
print(tn.read_until(b";"))


tn.close() #close the telnet connection