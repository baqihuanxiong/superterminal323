from socket import *
import threading
import time


HOST = '192.168.1.100'
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
                    data = cmd[0]+' '+f.read()
            if cmd[1] == "repeat":
                data = cmd[0]+' '+cmd[2]
                try:
                    while True:
                        tcpCliSock.send((data+'\r\n').encode('utf-8'))
                        time.sleep(float(cmd[3]))
                except KeyboardInterrupt:
                    pass
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
