import xmlrpclib
import socket


server = socket.socket()
server.bind(('', 10005))
server.listen(5)

while 1:
    c, addr = server.accept()
    print "new connection: %s" % str(addr)
    print str(dir(c))
    while 1:
        msg = c.recv(1024)
        if msg == '':
            break
        print msg
    print "connection closed"

server.close()
