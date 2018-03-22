import os
# This module provides access to the BSD socket interface.
import socket
# Python module for interacting with the `IBM Watson IoT Platform
import ibmiotf.device
# get the address(es) of the machine’s network interfaces LINUX
import netifaces as ni
# import modules to use address IPv4, IPv6 and MAC.
from netifaces import AF_INET, AF_INET6, AF_LINK
# Modulo time used to generate a sleep time
import time

# Port used for ETH connection
port = 1234

# Dictionary: save the options given for your IBM Bluemix account
options_ibm_count = {
    "org": "your-org",
    "type": "your-type",
    "id": "your-id",
    "auth-method": "token",
    "auth-token": "your-auth-token",
    "clean-session": "true"
}

# Funtion to clear screen
def clearScreen():
    if os.name == "nt":
        os.system("cls")
    elif os.name == "posix":
        os.system("clear")
    else:
        print("No se puede limpiar la pantalla")

# Function: Connect to IBM Bluemix platform
def connect_to_bluemix(options):
    try:
        print("Connecting to IBM Bluemix Platform...")
        mqtt_client = ibmiotf.device.Client(options)
        mqtt_client.connect()
        time.sleep(5)
    except ibmiotf.ConnectionException  as e:
        print("Error to connect IBM Bluemix IoT Platform.\nThe system will work locally.\n")
    except Exception as e:
        print("Error to connect IBM Bluemix IoT Platform.\nThe system will work locally.\n")
    return mqtt_client

# Set a TCP Server
def eth_connection():
    # Check if is runnung over Linux "posix" or Windows "nt"
    if os.name == "nt":
		# Constructor of server from socket class using IPv4 address, TCP connection
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		# Get the IP address from the host
        ip = socket.gethostbyname(socket.gethostname())
        address = (ip, port)
		# Set IP and Port used for the TCP server
        server.bind(address)
		# The TCP server start listening. Up to 5 simultaneous connection are allowed.
        server.listen(5)  # number of simultaneous connections
        print("""[*] TCP Server:\n\t Started listening on 'ip : port -> {} : {}'""".format(ip, port))
        return server
    elif os.name == "posix":
        # Constructor of server from socket class using IPv4 address, TCP connection
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # Get the IP address from the interface ETH0
        ip = ni.ifaddresses('eth0')[AF_INET][0]['addr']
        address = (ip, port)
        # Set IP and Port used for the TCP server
        server.bind(address)
        # The TCP server start listening. Up to 5 simultaneous connection are allowed.
        server.listen(5)
        print("""[*] TCP Server:\n\t Started listening on 'ip : port -> {} : {}'""".format(ip, port))
        return server
    else:
        print("This software run over Lunix or Windows. Your device is not supported.")

clearScreen()
bluemix_client = connect_to_bluemix(options=options_ibm_count)
server = eth_connection()
# An accomplished connection returns a new socket object usable to send and receive data on the connection,
# and address is the address bound to the socket on the other end of the connection.
tcp_client, addr = server.accept()
# Print the IP address and por of the TCP client connected
print("[*] Got a connection from 'ip : port -> {} : {}'".format(addr[0], addr[1]))

while (True):
    data = tcp_client.recv(1024)
    data_str = data.decode("utf-8");
    print("Received data:", data_str)
    bluemix_client.publishEvent("status", "json", data_str)


