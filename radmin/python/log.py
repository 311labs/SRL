"""
module: radmin.log
Very simple logging facilities.  We would use logger module but I don't like how they are setup
and how they are c style string formatting verses python string formatting

Created by Ian Starnes 2007-11-27

"""

import os, thread, sys, commands, traceback, shutil
from datetime import datetime

class Logger(object):
    """docstring for Logger"""
    OFF = 0
    CRITICAL = 10
    ERROR = 20
    WARNING = 30
    INFO = 40
    DEBUG = 50
    ALL = 100
    
    def __init__(self, name, level=INFO, path=None, format="%(datetime)s %(name)s %(level)s: %(message)s", 
                 info_format="%(datetime)s %(name)s: %(message)s"):
        self.name = name
        self.format = format
        self.info_format = info_format
        self.level = level
        self.today = None
        self.path = path
        self.archive_path = "%s/old" % self.path
        
        if path != None:
            self.output = None
            self.filepath = "%s/%s.log" % (self.path, self.name.replace(' ', '_'))
            self._rotate(datetime.now())
        else:
            self.output = sys.stdout
        self.lock=thread.allocate_lock()
        self.last_newline = True
        self.write("INFO", "logger started with level: %s" % self.levelAsString(self.level))
    
    
    def _rotate(self, now):
        """if the logger is file based this will rotate the log daily"""
        if self.taday is None or self.today.day != now.day:
            self.today = now
            if self.output != None:
                self.output.close()
                # make sure the archive path exists
                if not os.path.exists(self.archive_path):
                    os.makedirs(self.archive_path)
                # now move/rename the file to its new location
                shutil.move(self.filepath, "%s/%s_%s.log" %(self.archive_path, self.name, self.today.strftime("%Y%m%d")))
            # now lets open our new log file
            if not os.path.exists(self.path):
                os.makedirs(self.path)
            self.output = open(self.filepath, 'a')
    
    def close(self):
        pass
        
    def levelAsString(self, level):
        if level >= Logger.DEBUG:
            return "DEBUG"
        elif level >= Logger.INFO:
            return "INFO"
        elif level >= Logger.WARNING:
            return "WARNING"
        elif level >= Logger.ERROR:
            return "ERROR"
        elif level >= Logger.CRITICAL:
            return "CRITICAL"
        
        return "ALL"    
    
    def write(self, level, msg, new_line=True):
        self.lock.acquire()
        if self.last_newline:
            out = {"level":level, "name":self.name, "datetime":datetime.now(), "message":msg}
            # if we are logging to a file check if it needs to be rotated
            if self.path != None:
                self._rotate(out["datetime"])
            # now do the logging
            if level != "INFO":
               self.output.write(self.format % out)
            else:
               self.output.write(self.info_format % out)
        else:
            self.output.write(msg)
        
        if new_line:
            self.output.write('\n')
        self.output.flush()
        self.last_newline = new_line
        self.lock.release()
        
    def setLevel(self, level):
        self.level = level
    
    def debug(self, msg, new_line=True):
        if self.level >= Logger.DEBUG:
            self.write("DEBUG", msg, new_line)
            
    def info(self, msg, new_line=True):
        if self.level >= Logger.INFO:
            self.write("INFO", msg, new_line)
    
    def warn(self, msg, new_line=True):
        self.warning(msg, new_line)
    
    def warning(self, msg, new_line=True):
        if self.level >= Logger.WARNING:
            self.write("WARNING", msg, new_line)
    
    def error(self, msg, new_line=True):
        if self.level >= Logger.ERROR:
            self.write("ERROR", msg, new_line)

    def critical(self, msg):
        if self.level >= Logger.CRITICAL:
            self.write("CRITICAL", msg, new_line)

    def dumpStack(self):
        traceback.print_exception(sys.exc_info()[0],sys.exc_info()[1], sys.exc_info()[2], file=self.output) 

    