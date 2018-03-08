from asyncore import dispatcher
from asynchat import async_chat
import socket
import asyncore
import time
from time import ctime

PORT = 5005
NAME = 'TestChat'

test_agv = "192.168.31.136"
cards = []
with open(r'C:\Users\lw390\Documents\GitHub\superterminal323\RFID ID.txt','r') as fh:
	cards = fh.readlines()
	cards = [line.strip() for line in cards]
zone = [
        "z48900159",
		"z15905123",
		"z12300119",
		"z11908082",
		"z08200080",
		"z08007109",
		"z10900107",
		"z10700172",
		"z17200405"
		"z40506435"]
zone_start = [int(i[1:4]) for i in zone]
part = 0


class ChatSession(async_chat):
    """
    A single session, which takes care of the communication with a single user.
    """

    def __init__(self, server, sock, name):
        super().__init__(sock)
        self.server = server
        self.set_terminator(b"\r\n")
        self.data = ""
        self.name = name
        self.part = 0
        self.push('Welcome to {}'.format(self.server.name).encode('utf-8'))
        print(self.name+'connected')

    def collect_incoming_data(self, data):
        self.data += data.decode('utf-8')

    def found_terminator(self):
        # 处理数据
        print("["+ctime()+"]"+self.name+":"+self.data)
        if self.data.startswith("@"):
            self.server.personal(self.data.split(' ',1)[0][1:], self.data.split(' ',1)[1])
        elif self.data.startswith("."):
            if self.data.split('.',1)[1] == 'list':
                line = ''
                for session in self.server.sessions:
                    line += session.name + '\n'
                self.server.personal(self.name, line)
        elif self.data.startswith("F:"):
            card_id = self.data.split(':',1)[1].strip()
            nextZone = getZone(card_id, self.part)
            if nextZone:
                print("push next zone: "+nextZone)
                self.part += 1
                self.server.personal(test_agv, nextZone)

        self.data = ""

    def handle_close(self):
        self.server.disconnect(self)
        async_chat.handle_close(self)


def getZone(id, part):
    index = 0
    try:
        index = cards.index(id)+1
    except ValueError:
        print("Unknown card")
    if index in zone_start:
        i = zone_start.index(index)
        if i==part:
            return zone[i+1]


class ChatServer(dispatcher):
    """
    A chat server with a single room.
    """

    def __init__(self, port, name):
        super().__init__()
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.set_reuse_addr()
        self.bind(('', port))
        self.listen(5)
        self.name = name
        self.sessions = []
        print('Server %s is running on %s' %(name, port))

    def disconnect(self, session):
        self.sessions.remove(session)

    def broadcast(self, line):
        for session in self.sessions:
            session.push((line+'\r\n').encode('utf-8'))

    def personal(self, session_name, line):
        for session in self.sessions:
            if session.name == session_name:
                session.push((line+'\r\n').encode('utf-8'))
                return
        print('No session name '+session_name)

    def handle_accept(self):
        conn, addr = self.accept()
        self.sessions.append(ChatSession(self, conn, addr[0]))




if __name__ == '__main__':
    s = ChatServer(PORT, NAME)
    try:
        asyncore.loop()
    except KeyboardInterrupt:
        print()
