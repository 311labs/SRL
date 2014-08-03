import commands
from StringIO import StringIO

def ping(ip):
    """Sends 1 ping to the specified ip address and check if a echo was received"""
    cmd = "ping -n -q -w 1 -c 1 " + ip
    res = commands.getoutput(cmd)
    if res.rfind("1 received") > 0:
        return True
    return False

def objToJson(obj, obj_name=None, output=None):
    """Generate JSON output from Python Objects"""    
    has_output = output != None
    if not has_output:
        output = StringIO()

    if obj_name != None:
        output.write('{"%s":' % obj_name)    
    
    if type(obj) in [float, int, long]:
        output.write(str(obj))
    elif type(obj) is bool:
        output.write(str(int(obj)))
    elif type(obj) is str:
        output.write('"%s"' % obj)
    elif type(obj) is list:
        output.write("[")
        is_first = True
        for item in obj:
            if not is_first:
                output.write(", ")
            else:
                is_first = False
            objToJson(item, None, output)
        output.write("]")
    elif type(obj) is dict:
        output.write('{')
        is_first = True
        for key in obj:
            if not is_first:
                output.write(", ")
            else:
                is_first = False
            output.write('"%s":' % key)
            objToJson(obj[key], None, output)
        output.write('}')
    
    elif obj is None:
        output.write("null")
    else:
        objToJson(obj.__dict__, None, output)

    if obj_name != None:
        output.write('}')
    
    if not has_output:
        out = output.getvalue()
        return out


def appendToFile(filename, data):
    f = open(filename, "a")
    f.write(data)
    f.close()

def writeFile(filename, data):
    f = open(filename, "w")
    f.write(data)
    f.close()


