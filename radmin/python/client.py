"""
module: radmin.client
Radmin client logic that is used to interface with the Radmin clients

Created by Ian Starnes 2007-11-27

"""

from threading import Thread
import thread, time, commands, os
import timeout_xmlrpclib as xmlrpclib
import socket, select
# radmin modules
import config, log

class Triggers(object):
    """docstring for Triggers"""
    STOP_MONITOR=1
    REMOVE_TRIGGER=2
    NOTIFY_INFO=4
    NOTIFY_WARN=8
    NOTIFY_ERROR=16
    
    def __init__(self):
        self.triggers = []
    
    def addKeywordTrigger(self, keyword, actions):
        """adds a new trigger based on the keyword and the desired actions"""
        self.triggers.append({"name":keyword, "keyword":keyword, "actions":actions})

# value triggers should also be done by field name not id?


class Client(object):
    """docstring for Node"""
    def __init__(self, connection, logger):
        """takes a socket connection and the ip the connection is from"""
        self.lock=thread.allocate_lock()
        self.con = connection
        self.addr = self.con.getpeername()
        self.control_ip = self.addr[0]
        self.rpc_port = None
        self.logger = logger
        self.hostname = ""
        self.monitored_pids = []
        self.event_data = None
        self.end_offset = len("</methodResponse>")
        self.is_ready = False
        # we must wait until we get the client info
        for i in range(0, 10):
            self._readEvent()
            if self.rpc_port != None:
                break
            time.sleep(1.0)
        if self.rpc_port is None:
            raise Exception("failed to receive client header")
        self.rpc = xmlrpclib.Server("http://%s:%d" % (self.control_ip, self.rpc_port))

    def __str__(self):
        return "<Radmin.Client: %s>" % self.control_ip

    def client_disconnected(self):
        """called when a client disconnects"""
        pass
        
    def client_error(self, error):
        """called when an error occurs during a recieve"""
        self.logger.error("client(%s) error: '%s" % (str(self), str(error)))


    def monitor_keyword(self, keyword, match, actions, event):
        """called when a monitor keyword matches"""
        self.logger.debug("montir(%s) keyword: %s '%s'" % (event["monitor"], keyword, match))
    
    def monitor_stopped(self, event):
        """called when a monitor stops"""
        pass

    def process_keyword(self, pid, keyword, match, actions, event):
        """called when a process being monitored as a keyword trigger event"""
        self.logger.debug("process(%d) keyword: %s '%s'" % (pid, keyword, match))

            
    def process_exit(self, pid, name):
        """called when a process being monitored exits"""
        self.logger.debug("monitored process(%s - %d) has exited" % (name, pid))
        if self.monitored_pids.count(pid) > 0:
            self.monitored_pids.remove(pid)


    def radmin_event(self, event):
        """
            different types of events:
            defaut fields:
                event   this field should exists on all events and tells us the type of event
            
            
            trigger
                fields:
                    monitor     name of monitor this trigger is from
                    monitor_type    type of monitor 
                    trigger     name of the trigger
                    trigger_type        type of trigger (basic, keyword, value)
                    actions     actions this triggered
                    if type == keyword:
                        keyword     keyword for this trigger
                        match       data that the keyword matched on
            stopped
                fields:
                    monitor     name of the monitor that stopped
                    monitor_type    type of the monitor
                    
            
        """
        
        # should call client listeners or be overriden?
        if self.is_ready:
            eventstr = "unknown"
            if event.has_key("event"):
                eventstr = event["event"]
                self.lock.acquire()
                
                self.logger.debug("%s monitor(%s) event: %s" % (self.control_ip, event["monitor"], eventstr))
                if eventstr == "stopped":
                    if event.has_key("pid"):
                        self.process_exit(event["pid"], event["monitor"])
                    else:
                        self.monitor_stopped(event)
                elif eventstr == "trigger":
                    if event["trigger_type"] == "keyword":
                        if event.has_key("pid"):
                            self.process_keyword(event["pid"], event["keyword"], event["match"], event["actions"], event)
                        else:
                            self.monitor_keyword(event["keyword"], event["match"], event["actions"], event)
                
                self.lock.release()
        else:
            if event.has_key("hostname"):
                self.is_ready = True
                self.hostname = event["hostname"]
                self.os = {"name":event["os_name"], "kernel":event["os_kernel"], "version":event["os_version"]}
                self.cpu = {"model":event["cpu_model"], "speed":event["cpu_speed"], "count":event["cpu_count"], 
                            "physical":event["cpu_physical"], "logical":event["cpu_logical"], "cores":event["cpu_cores"]}
                self.memory_total = event["memory_total"]
                self.network = event["network"]
                self.rpc_port = event["rpc_port"]
                self.logger.debug("host: %s (%s %s)  cpu: %s %0.2fx%d" % (self.hostname, self.os["kernel"], self.os["version"],
                    self.cpu["model"], self.cpu["speed"], self.cpu["count"]))


    def _parseEvents(self, data):
        """this function will parse any events out of the received data"""
        while len(data) > 0:
            spos = data.find("<methodRes")
            if spos == -1:
                return data
                # if data.find("<?") == -1:
                #     self.logger.warn("throwing away bad event data: '%s'" % data)
                #     return None;
                # return data
            # now lets try and find the end tag
            epos = data.find("</methodResponse>")
            if epos == -1:
                # no end tag found so return all the data
                return data
            # else epos >= 0
            epos = epos + self.end_offset
            xml = data[spos:epos]
            # now data is what is ever left
            data = data[epos:].strip()
            try:
                self.radmin_event(xmlrpclib.loads(xml)[0][0])
            except Exception, e:
                self.client_error(e)
                self.logger.dumpStack()
                self.logger.warn("\n\n%s\n\n" % xml)
        return None

    def _readEvent(self):
        # returns False if the socket is closing
        # now lets read the data
        # all data from a radmin client should be in xml-rpc response format
        # read the first line if it does not start with <xml or <methodResponse then just return
        data = self.con.recv(9216)
        if len(data) == 0:
            return False        
        
        if self.event_data is None:
            self.event_data = self._parseEvents(data)
        else:
            self.event_data = self._parseEvents(self.event_data + data)
        
        return True
        
        
    def getSystemInfo(self):
        """returns a dictionary of remote system information"""
        return self.rpc.system.getInfo()
    
    def getStats(self):
        """returns the current cpu and memory load"""
        return self.rpc.system.getUsage()
    
    def getstatusoutput(self, command, user=None):
        """runs a remote command and blocks until the command exits returning the output"""
        res = None
        if user:
            res = self.rpc.system.getStatusOutput(command, user)
        res = self.rpc.system.getStatusOutput(command)    
        return res["status"], res["output"]
    
    def getoutput(self, command, user=None):
        """runs a remote command and blocks until the command exits returning the output"""
        if user:
            return self.rpc.system.getCommandOutput(command, user)
        return self.rpc.system.getCommandOutput(command)
    
    def runCommands(self, commands):
        """runs a list of remote commands returning the results as a list"""
        return self.rpc.system.runCommands(commands)
    
    def findProcess(self, name):
        """returns list of processes that match the passed in name"""
        return self.rpc.system.findProcess(name)
        
    def getProcessInfo(self, pid):
        """returns the info a specified process by its pid"""
        return self.rpc.system.getProcess(pid)
    
    def listProcesses(self, user=None):
        """returns a list of all the current running processes"""
        if user:
            return self.rpc.system.listProcesses(user)
        return self.rpc.system.listProcesses()
    
    def killAllProcs(self, user):
        """will kill all the processes that are owned by the user"""
        try:
            self.rpc.radmin.killAllProcs(user)
        except Exception, e:
            self.client_error(e)
            
    def killProcess(self, pid):
        """kills a process based on its pid"""
        return self.rpc.system.killProcess(pid)
    
    def monitorProcess(self, command, user=None, triggers=None, logfile=None):
        """runs a remote command and monitors the output using the defined triggers"""
        # FIXME check if it did not start
        monproc = {"command":command}
        if user != None:
            monproc["user"] = user
        if triggers != None:
            monproc["triggers"] = triggers.triggers
        if logfile != None:
            monproc["logfile"] = logfile
        self.lock.acquire()
        pid = None
        try:
            pid = self.rpc.radmin.monitorProcess(monproc)
            self.monitored_pids.append(pid)
        except Exception, e:
            self.logger.error(str(e))
        self.lock.release()
        return pid

    def pathExists(self, path):
        """checks if the path exists"""
        return self.rpc.system.pathExists(path)
        
    def writeToFile(self, filename, text, user="", append=False):
        """write a text file"""
        return self.rpc.system.writeToFile(filename, text, user, append)
    
    def catFile(self, filename):
        return self.rpc.system.catFile(filename)

    def makeDir(self, filename, owner=None):
        if owner is None:
            return self.rpc.system.makeDir(filename)
        return self.rpc.system.makeDir(filename, owner)
        
    def removeDir(self, filename):
        return self.rpc.system.removeDir(filename)
    
    def sendFile(self, local_filename, remote_filename, owner=None):
        """sends a local file to the remove destination path"""
        ft_port = 0
        if owner:
            ft_port = self.rpc.radmin.sendFile(remote_filename, owner)
        else:
            ft_port = self.rpc.radmin.sendFile(remote_filename)

        if type(ft_port) is int:
            f = open(local_filename, 'r')
            err = None
            try:
                client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                client_socket.connect((self.control_ip, ft_port))
                while True:
                    data = f.read(255)
                    #print "sent %d bytes" % len(data)
                    if data is None or len(data) == 0:
                        break
                    client_socket.send(data)
            except Exception, e:
                err = e
            client_socket.close()
            f.close()
            if err != None:
                raise err
            return True
        return False

    def getFile(self, remote_filename, local_filename):
        """downloads a remote file to the local file destination"""
        ft_port = self.rpc.radmin.getFile(remote_filename)
        if type(ft_port) is int:
            f = open(local_filename, 'w')
            err = None
            try:
                client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                client_socket.connect((self.control_ip, ft_port))
                while True:
                    data = client_socket.recv(255)
                    #print "received %d bytes" % len(data)
                    if data is None or len(data) == 0:
                        break
                    f.write(data)
            except Exception, e:
                err = e
            client_socket.close()
            f.close()
            if err != None:
                raise err
            return True
        return False
    
    def convertPath(self, path):
        """converts the path to the correct format of the client"""
        if self.os["name"].lower() == "windows":
            return path.replace('/', '\\')
        return path.replace('\\', '/')
    
    def sendFolder(self, local_folder, remote_folder, owner=None):
        """will mirror a local folder to a remote folder"""
        for root, folders, files in os.walk(local_folder):
            sub_root = root.replace(local_folder, "")
            for folder in folders:
                remote_item = self.convertsPath(os.path.join(remote_folder, sub_root, folder))
                self.makeDir(remote_item, owner)
            
            for f in files:
                remote_item = self.convertsPath(os.path.join(remote_folder, sub_root, f))
                local_item = os.path.join(root, f)
                self.sendFile(local_item, remote_folder, owner)
        
        
class ClientPool(Thread):
    """
    The client pool listens for radmin clients and listens for incoming data on them
    """
    def __init__(self, config, ClientClass=Client, logger=None):
        Thread.__init__(self)
        if logger is None:
            logger = log.Logger("radmin")
        self.logger = logger
        self.host = config.get("event_host", "")
        self.port = config.get("event_port", 8080)
        self.clients = {}
        self.remove_list = []
        self.inputs = []
        self.stop_flag = False
        self.listeners = []
        self.ClientClass = ClientClass
        self.server = None
    
    def __iter__(self):
        """iterate through the list of clients"""
        return iter(self.clients.values)
    
    def size(self):
        """returns the number of clients in the pool"""
        return len(self.clients)
    
    def addListener(self, listener):
        self.listeners.append(listener)
    
    def removeListener(self, listener):
        self.listeners.remove(listener)

    def client_added(self, client):
        for listener in self.listeners:
            listener.client_added(client)

    def client_removed(self, client):
        for listener in self.listeners:
            listener.client_removed(client)
    
    def getClient(self, index=0):
        """returns the client at index(default last item)"""
        if len(self.clients):
            return self.clients.values()[index]
        return None
        
    def stop(self):
        """docstring for stop"""
        if self.server:
            self.server.close()
            self.stop_flag = True
            self.server = None
            
    def new_connection(self, con_tup):
        """docstring for new_connection"""
        con, addr = con_tup
        try:
            if self.clients.has_key(addr[0]):
                self.logger.warn("multiple connections from the same client are not allowed")
                self.logger.warn("removing old connection...")
                self._removeClient(self.clients[addr[0]])
                self.logger.warn("old connection removed adding new connection")
            self.logger.info("new connection: %s" % str(con.getpeername()))
            client = self.ClientClass(con, self.logger)
            self.clients[addr[0]] = client
            self.inputs.append(con)
            self.client_added(client)
        except Exception, e:
            errmsg = str(e)
            if errmsg.find("not connected") > 0:
                self.logger.error("connection(%s) is not real" % addr[0])
            self.logger.error("Client(%s) failed" % addr[0])
            self.logger.dumpStack()
            con.close()

    def remove(self, client):
        self.remove_list.append(client)
    
    def _removeClient(self, client):
        client.con.close()
        client.client_disconnected()
        self.inputs.remove(client.con)
        del self.clients[client.control_ip]
        self.client_removed(client)
                    
    def run(self):
        """called by thread with the sole purpose of checking for incoming events"""
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server.bind((self.host, self.port))
        self.server.listen(50)
        self.inputs.append(self.server)
        self.logger.info("now listening for events on: %s:%d" % (self.host, self.port))
        while not self.stop_flag:
            # black forever or until we need to do something
            inputready,outputready,exceptready = select.select(self.inputs, [], [], 5.0)
            
            # remove any old clients first
            if len(self.remove_list) > 0:
                for client in self.remove_list:
                    self.logger.warn("removing client by request '%s'" % client.control_ip)
                    self._removeClient(client)
                self.remove_list = []
            
            for s in inputready:
                if s == self.server:
                    self.new_connection(self.server.accept())
                    if self.stop_flag:
                        return
                elif self.clients.has_key(s.getpeername()[0]):
                    client = self.clients[s.getpeername()[0]]
                    if not client._readEvent():
                        self.logger.info("client(%s) has disconnected" % client.control_ip)
                        self._removeClient(client)
                else:
                    self.logger.warn("unknown input ready from select, maybe socket is not in our client list?")
            
            for s in exceptready:
                if s == self.server:
                    self.logger.error("server socket reporting error")
                    return
                elif self.clients.has_key(s.getpeername()[0]):
                    client = self.clients[s.getpeername()[0]]
                    self.logger.info("client(%s) has errors removing" % client.control_ip)
                    self._removeClient(client)

class TestClient(Client):
    """cient for testing basic radmin functionality"""
    def __init__(self, connection, logger):
        Client.__init__(self, connection, logger)
        
    def runTests(self):
        """runs a set of exensive tests"""
    
        # first test file exists functionality
        if not self.pathExists("/etc/hosts"):
            raise Exception("client.pathExists failed for '/etc/hosts'")
    
        hostname = self.getoutput("hostname").strip()
        if hostname != self.hostname:
            raise Exception("client.getoutput failed running 'hostname' got '%s' expected '%s'" % (hostname, self.hostname))
        
        triggers = Triggers()
        triggers.addKeywordTrigger("icmp", Triggers.NOTIFY_WARN)
        triggers.addKeywordTrigger("transmitted", Triggers.NOTIFY_WARN)
        mproc = {"command":"bash -l -c 'ping -c 10 www.google.com'", "triggers":triggers.triggers}
        self.monitorProcess("bash -l -c 'ping -c 10 www.google.com'", triggers=triggers, logfile= "/tmp/ians.log")
        
        
    def monitor_keyword(self, keyword, match, actions, event):
        """called when a monitor keyword matches"""
        self.logger.debug("montir(%s) keyword: %s '%s'" % (event["monitor"], keyword, match))
        

    def monitor_stopped(self, event):
        """called when a monitor stops"""
        pass

    def process_keyword(self, pid, keyword, match, actions, event):
        """called when a process being monitored as a keyword trigger event"""
        self.logger.debug("process(%d) keyword: %s '%s'" % (pid, keyword, match))


    def process_exit(self, pid, name):
        """called when a process being monitored exits"""
        self.logger.debug("monitored process(%s - %d) has exited" % (name, pid))
        if self.monitored_pids.count(pid) > 0:
            self.monitored_pids.remove(pid)    



def hammerClient(client, hits=40):
    """hammers a client with a ton of threads"""
    import threading
    for i in range(0, hits):
        t = threading.Thread(target=client.runTests)
        t.start()
            

if __name__ == "__main__":
    import sys
    pool = ClientPool(config.Config(sys.argv[1]), ClientClass=TestClient, logger=log.Logger("radmin", level=log.Logger.DEBUG))
    pool.start()
    

