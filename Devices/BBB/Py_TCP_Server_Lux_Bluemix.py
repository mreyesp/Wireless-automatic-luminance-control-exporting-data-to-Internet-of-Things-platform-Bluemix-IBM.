import os
import socket
import ibmiotf.device
import netifaces as ni
from netifaces import AF_INET, AF_INET6, AF_LINK

def clearScreen():
    if os.name == "nt":
        os.system("cls")
    elif os.name == "posix":
        os.system("clear")
    else:
        print("Can not clear screen")

clearScreen()

try:
    options = {
        "org": "your-org",
        "type": "your-type",
        "id": "your-id",
        "auth-method": "your-auth-method",
        "auth-token": "your-auth-token",
        "clean-session": "true"
    }
    print("Connecting to IBM Bluemix Platform...")
    mqtt_client = ibmiotf.device.Client(options)
    mqtt_client.connect()
except ibmiotf.ConnectionException  as e:
    print("Error to connect IBM Bluemix IoT Platform.\nThe system will work locally.\n")
except Exception as e:
    print("Error to connect IBM Bluemix IoT Platform.\nThe system will work locally.\n")

server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
#ip = socket.gethostbyname(socket.gethostname())
ip = ni.ifaddresses('eth0')[AF_INET][0]['addr']
port = 1234
address = (ip,port)
server.bind(address)
server.listen(5) #number of simultaneous connections
print ("""[*] TCP Server:\n\t Started listening on 'ip : port -> {} : {}'""".format(ip,port))
