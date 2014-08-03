#include "srl/io/BinaryReader.h"

using namespace SRL;
using namespace SRL::IO;

int BinaryReader::read(byte *obj, uint32 arrysize)
{
	return read(obj, sizeof(byte), arrysize);
}

int BinaryReader::read(int16 *obj, uint32 arrysize)
{
	return read(obj, sizeof(int16), arrysize);
}

int BinaryReader::read(int32 *obj, uint32 arrysize)
{
	return read(obj, sizeof(int32), arrysize);
}

int BinaryReader::read(int64 *obj, uint32 arrysize)
{
	return read(obj, sizeof(int64), arrysize);
}

int BinaryReader::read(uint16 *obj, uint32 arrysize)
{
	return read(obj, sizeof(uint16), arrysize);
}

int BinaryReader::read(uint32 *obj, uint32 arrysize)
{
	return read(obj, sizeof(uint32), arrysize);
}

int BinaryReader::read(uint64 *obj, uint32 arrysize)
{
	return read(obj, sizeof(uint64), arrysize);
}

int BinaryReader::read(float32 *obj, uint32 arrysize)
{
	return read(obj, sizeof(float32), arrysize);
}

int BinaryReader::read(float64 *obj, uint32 arrysize)
{
	return read(obj, sizeof(float64), arrysize);
}

int BinaryReader::read(bool *obj, uint32 arrysize)
{
	return read(obj, sizeof(bool), arrysize);
}

int BinaryReader::read(char *str, uint32 arrysize)
{
    int read_count = 0;
	// read the size of the field in
	uint32 strsize;
	if (arrysize == 0)
		read_count = read(&strsize, sizeof(uint32), 1);
	else
		strsize = arrysize;

	if ((strsize > 0) && (strsize < 512))
	{
		read_count += read(str, sizeof(char), strsize);
		str[strsize] = '\0';
	}
	else
		str[0] = '\0';
    return read_count;
}

int BinaryReader::read(String &str, uint32 arrysize)
{
    int read_count = 0;
	uint32 strsize;
	if (arrysize == 0)
		read_count = read(&strsize, sizeof(uint32), 1);
	else
		strsize = arrysize;

	if (strsize > 0)
	{
		str.resize(strsize);
		read_count += read(str._strPtr(), sizeof(char), strsize);
		str[strsize] = '\0';
	}
	else
	{
		str.clear();
	}
	return read_count;
}
/*
int BinaryReader::read(Serializable *obj)
{
    return obj->deserialize(this);
}
*/
