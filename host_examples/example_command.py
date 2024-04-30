import telnetlib 
tn = telnetlib.Telnet("192.168.1.177", 23)
tn.write(b"?1\n")
output = tn.read_all() 
print(output) 