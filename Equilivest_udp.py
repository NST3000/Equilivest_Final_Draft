import socket, json, random


localIP     = "xxxxxx"

localPort   = 20001

bufferSize  = 1024

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM) 

#UDPServerSocket.close()
  
# Bind to address and ip

UDPServerSocket.bind((localIP, localPort)) 

print("UDP server up and listening")

# Save to file

file_id = str(random.randint(-9999999,9999999))
file = open(file_id+".txt", "a")
file.write("pitch, roll, yaw, gyroy, gyrox, gyroz, count\n")

# Listen for incoming datagrams

i = 0

while(True):

    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)

    message = bytesAddressPair[0]
    
    address = bytesAddressPair[1]
    
    msg_decode = message.decode("utf-8")
    
    signals = msg_decode.split("=")
    
    pitch = signals[0]
    
    roll = signals[1]
    
    yaw = signals[2]
    
    gyroy = signals[3]
    
    gyrox = signals[4]
    
    gyroz = signals[5]
    
    count = signals[6]
    
    file.write("{},{},{},{},{},{},{}\n".format(pitch, roll, yaw, gyroy, gyrox, gyroz, count))
    
    file.flush()
    
    print("Message from Equilivest: \n pitch: {}, roll: {}, yaw: {}, gyroy: {}, gyrox: {}, gyroz: {}, count: {}".format(pitch, roll, yaw, gyroy, gyrox, gyroz, count))
    
    UDPServerSocket.sendto(str.encode(""), address)
    
file.close()
