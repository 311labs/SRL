# @author Ian Starnes
# @date March 19, 2008
#
# This code was taken from an article by: Volodymyr Orlenko
import xmlrpclib
import httplib

loads = xmlrpclib.loads

class TimeoutTransport(xmlrpclib.Transport):

    def make_connection(self, host):
        conn = TimeoutHTTP(host)
        conn.set_timeout(self.timeout)
        return conn

def Server(url, *args, **kwargs):
    t = TimeoutTransport()
    t.timeout = kwargs.get('timeout', 120)
    if 'timeout' in kwargs:
        del kwargs['timeout']
    kwargs['transport'] = t
    server = xmlrpclib.Server(url, *args, **kwargs)
    return server
    
class TimeoutHTTPConnection(httplib.HTTPConnection):

    def connect(self):
        httplib.HTTPConnection.connect(self)
        self.sock.settimeout(self.timeout)


class TimeoutHTTP(httplib.HTTP):
    _connection_class = TimeoutHTTPConnection

    def set_timeout(self, timeout):
        self._conn.timeout = timeout
        
