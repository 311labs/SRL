"""
module: radmin.confg
Config file logic

Created by Ian Starnes 2007-11-27

"""

class Config(object):
    """The Config class."""
    def __init__(self, filename):
        self.filename = filename
        self._items = {}
        execfile(filename, globals(), self._items)
    
    def add(self, key, value):
        self._items[key] = value
        
    def has_key(self, key):
        return self._items.has_key(key)
    
    def get(self, field, default=None, required=False):
        "returns the field as a string"
        if self._items.has_key(field):
            return self._items[field]
        if required:
            raise Exception("missing required field(%s)!" % field)
        return default

class IniFile(object):
    """docstring for IniFile"""
    SYNTAX_COMMENTS = ['#', ';'] 
    SYNTAX_DELIMITER = '='
        
    def __init__(self, filename):
        self.filename = filename
        self.sections = []
        self._section_dict = {}
    
    def hasSection(self, section):
        return section in self.sections
            
    def getSection(self, section):
        if section not in self.sections:
            self._section_dict[section] = {}
            self.sections.append(section)
        return self._section_dict[section]
    
    def load(self):
        f = open(self.filename, 'r')
        section = self.getSection("")
        quote = None
        key = None
        value = None
        for line in f.readlines():
            # if inside a quote read anything until end quote
            if quote != None:
                new_value = line.strip()
                if new_value[-1] == quote:
                    value += new_value[:-1]
                    quote = None
                    section[key] = value
                    continue
                value += new_value
                
            # ignore comments or empty lines
            if line[0] in IniFile.SYNTAX_COMMENTS or len(line) < 3:
                continue
            
            # check for new section
            if line[0] == '[':
                section = self.getSection(line[1:line.find("]")])
                continue
                
            # check for delimiter
            if line.count(IniFile.SYNTAX_DELIMITER) >= 1:
                #key, value = line.split('=')
                pos = line.find(IniFile.SYNTAX_DELIMITER)
                key = line[:pos].strip()
                value = line[pos+len(IniFile.SYNTAX_DELIMITER):].strip()
                if value[0] == '"':
                    quote = '"'
                elif value[0] == "'":
                    quote = "'"
                
                # parse the string
                if quote != None:
                    if value[-1] == quote:
                        value = value[1:-1]
                        quote = None
                    else:
                        value = value[1:]
                        continue
                else:
                    if value.count('.') == 1:
                        flt = value.split('.')
                        if flt[0].isdigit() and flt[1].isdigit():
                            value = float(value)
                    elif value.isdigit():
                        value = int(value)
                section[key] = value
                    
            else:
                print "no delimiter found in line: %s" % line
    
    def _write(self, writer, comment=None):
        f = writer
        if comment:
            f.write("# %s\n\n" % self.comment)
        for name in self.sections:
            section = self._section_dict[name]
            if len(section) > 0:
                f.write("[%s]\n" % name)
                sorted_keys = section.keys()
                sorted_keys.sort()
                for key in sorted_keys:
                    value = section[key]
                    if type(value) is str:
                        if value.count('\n') > 0 or value.count('"') > 0:
                            f.write('%s="""%s"""\n\n' % (key, value))
                        else:
                            f.write('%s="%s"\n' % (key, value))
                    else:
                        f.write('%s=%s\n' % (key, value))
                f.write("\n")
    
    def save(self, comment=None):
        f = open(self.filename, 'w')
        self._write(f, comment)
        f.close()
    
    def __str__(self):
        import StringIO
        writer = StringIO.StringIO()
        self._write(writer)
        out = writer.getvalue()
        writer.close()
        return out
        
    def asHTML(self):
        """generate html output"""
        import StringIO, cgi
        f = StringIO.StringIO()
        f.write("<div class='ini'>\n")
        for name in self.sections:
            section = self._section_dict[name]
            if len(section) > 0:
                error_summary = False
                if name.lower().count("error") > 0:
                    error_summary = True
                f.write("<div class='ini_section'>\n")
                
                count = 0
                f.write("<h3>%s</h3>\n" % name)
                if error_summary:
                    f.write("<div class='ini_errors'>\n")
                sorted_keys = section.keys()
                sorted_keys.sort()
                for key in sorted_keys:
                    value = section[key]
                    if not error_summary:
                        f.write("<span class='ini_key'>%s:</span> <span class='ini_value'>%s</span>\n" %(key, value))
                    else:
                        f.write("<span class='ini_error'>%s</span>\n" % cgi.escape(value))
                    count += 1
                    if count == 3 or error_summary:
                        f.write("<br />\n")
                        count = 0
                if error_summary:
                    f.write("</div>\n")                
                f.write("</div>\n")
        f.write("</div>\n")        
        content = f.getvalue()
        f.close()
        return content
        
        