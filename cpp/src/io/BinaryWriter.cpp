#include "srl/io/BinaryWriter.h"

#include <string.h>

using namespace SRL;
using namespace SRL::IO;

int BinaryWriter::write(const byte *obj, uint32 arrysize)
{
	return writeData(obj, sizeof(byte), arrysize);
}

int BinaryWriter::write(const int16 *obj, uint32 arrysize)
{
	return writeData(obj, sizeof(int16), arrysize);
}

int BinaryWriter::write(const int32 *obj, uint32 arrysize)
{
	return writeData(obj, sizeof(int32), arrysize);
}

int BinaryWriter::write(const int64 *obj, uint32 arrysize)
{
	return writeData(obj, sizeof(int64), arrysize);
}

int BinaryWriter::write(const uint16 *obj, uint32 arrysize)
{
	return writeData(obj, sizeof(uint16), arrysize);
}

int BinaryWriter::write(const uint32 *obj, uint32 arrysize)
{
	return writeData(obj, sizeof(uint32), arrysize);
}

int BinaryWriter::write(const uint64 *obj, uint32 arrysize)
{
	return writeData(obj, sizeof(uint64), arrysize);
}

int BinaryWriter::write(const float32 *obj, uint32 arrysize)
{
	return writeData(obj, sizeof(float32), arrysize);
}

int BinaryWriter::write(const float64 *obj, uint32 arrysize)
{
	return writeData(obj, sizeof(float64), arrysize);
}

int BinaryWriter::write(const SRL::byte& b)
{
    return writeData(&b, 1, 1);
}

int BinaryWriter::write(const bool *obj, uint32 arrysize)
{
	return writeData(obj, sizeof(bool), arrysize);
}

int BinaryWriter::write(const char *str, bool writeLength)
{
	uint32 size = strlen(str);
	int write_count = 0;
	if (writeLength)
	{
		write_count = writeData(&size, sizeof(uint32), 1);
	}
	write_count += writeData(str, sizeof(char), size);
    return write_count;
}

int BinaryWriter::write(const String &str, bool writeLength)
{
	return write(str.c_str(), writeLength);
}

/*
int BinaryWriter::write(Serializable *obj)
{
    return obj->serialize(this);
}
*/
