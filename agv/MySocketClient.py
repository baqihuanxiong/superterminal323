from socket import *
import threading


HOST = '192.168.31.164'
PORT = 5005
BUFSIZ = 1024
ADDR = (HOST, PORT)

tcpCliSock = socket()
tcpCliSock.connect(ADDR)


def send():
    while True:
        data = input('> ')
        if not data:
            continue
        cmd = data.split(' ')
        if len(cmd) > 1:
        	if cmd[1] == "readtxt":
	        	with open(data.split(' ')[2],'r') as f:
	        		data = data.split(' ')[0]+' '+f.read()
        tcpCliSock.send((data+'\r\n').encode('utf-8'))


def receive():
    while True:
        rec = tcpCliSock.recv(BUFSIZ)
        if rec:
            print(rec.decode('utf-8'))


t1 = threading.Thread(target=receive)
t2 = threading.Thread(target=send)
t1.start()
t2.start()
t1.join()
t2.join()

