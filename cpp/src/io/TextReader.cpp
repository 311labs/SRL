#include "srl/io/TextReader.h"

using namespace SRL;
using namespace SRL::IO;

int TextReader::read(String &buf, int len, int msec)
{
	buf.resize(len);
	int received = read((char*)buf._strPtr(), len*sizeof(char), msec);
	buf.resize(received);
	return received;    
}

