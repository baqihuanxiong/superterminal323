from socket import *
import threading


HOST = '192.168.31.164'
PORT = 5005
BUFSIZ = 1024
ADDR = (HOST, PORT)

tcpCliSock = socket()
tcpCliSock.connect(ADDR)

card_id = ""


def receive():
    while True:
        rec = tcpCliSock.recv(BUFSIZ)
        if rec:
        	cmd = rec.decode('utf-8').split(':')
        	if len(cmd)==2:
        		if cmd[0].strip() == 'F':
            		card_id = rec.split(':')[1].strip()


def send_timed():



t1 = threading.Thread(target=receive)
t1.start()
t1.join()

