#include "srl/io/TextWriter.h"

using namespace SRL;
using namespace SRL::IO;

int TextWriter::write(const String& msg)
{
	return write(msg.text(), msg.size());    
}

int TextWriter::writeLine(const String& msg)
{
	return write(msg.text(), msg.size()) + write(EOL_STRING);    
}

