
#include "srl/io/ByteBuffer.h"

#include <cstdlib>
#include <string.h>

using namespace SRL;
using namespace SRL::IO;
using namespace SRL::Errors;


// The byte buffer is always rounded to a multiple of ROUNDVAL
// which must be 2^n.  Thus, small size changes will not result in any
// actual resizing of the buffer except when ROUNDVAL is exceeded.
#define ROUNDVAL    16

// This will come in handy
#define EMPTY       ((byte*)&emptybuffer[1])

static const SRL::integer emptybuffer[2]={0,0};

// Change the length of the string to len
void ByteBuffer::resize(uint32 new_size)
{
    // set the string length the to the actual desired length
    //_length = new_size;
    
    // check if the string length is greater then 0
    if (new_size>0)
    {
        // find the new array size and make sure its atleast 1 bigger then the string size
        new_size = ROUNDUP(new_size+1, ROUNDVAL);
        // check if the current length is different from the new length
        if(new_size == _size)
        {
            if (new_size > 0)
                _buffer[new_size-1] = 0;
            return;
        }   

        //check if the data is currently empty
        if (_buffer == EMPTY)
        {
            // simple malloc (rounding up to the nearest 16th byte)
            _buffer = (byte*)malloc(new_size*sizeof(byte));
        }
        else
        {
            _buffer = (byte*)realloc(_buffer, new_size*sizeof(byte));
        }
        // null the string
        _buffer[new_size-1] = 0;

    } // if the current _buffer is empty do nothing
    else if (_buffer != EMPTY)
    {
        // set the size to 0
        new_size = 0;
        // free the memory
        free(_buffer);
        // set the buffer to empty
        _buffer = EMPTY;
    }
    // set the data size to the new size
    _size = new_size;

}

ByteBuffer::ByteBuffer(uint32 new_size) :
BinaryReader(),
BinaryWriter(),
_buffer(EMPTY),
_size(0),
_length(0),
_pos(0)
{
    resize(new_size);
}
ByteBuffer::ByteBuffer(byte* buffer, uint32 buf_size, uint32 buf_length) :
BinaryReader(),
BinaryWriter(),
_buffer(buffer),
_size(buf_size),
_length(buf_length),
_pos(0)
{
    if (buf_size == 0)
        _size = sizeof(_buffer);
}

ByteBuffer::ByteBuffer(const ByteBuffer& buffer) : 
BinaryReader(),
BinaryWriter(),
_buffer(EMPTY), _size(0), 
_length(0)
{
    register uint32 len = buffer.size();
    resize(len);
    if (len > 0)
    {
        memcpy(_buffer, buffer.bytes(), sizeof(byte)*len);
    }
}

ByteBuffer::~ByteBuffer()
{
    resize(0);
}

void ByteBuffer::toggle(bool write_mode)
{
    if (write_mode)
    {
        if (_pos == _length)
        {
            _pos = 0;
            _length = 0;
        }
        else
        {
            moveStart();
        }
    }
    else
    {
        _pos = 0;
    }
}

void ByteBuffer::flip()
{
    setPosition(0);
}

void ByteBuffer::setPosition(const uint32& pos)
{
    if (pos > _size)
        throw IndexOutOfBoundsException("past bytebuffer allocated size");
    _pos = pos;
}

void ByteBuffer::setLength(const uint32& length)
{
    _length = length;
    //_buffer[_length] = '\0';
}

void ByteBuffer::moveStart()
{
    if ((_pos < _length) && (_length > 0) && (_pos >= 0))
    {
        _length = _length-_pos;
        ::memmove(_buffer, &_buffer[_pos], _length);
        _pos = _length;
        _buffer[_pos] = '\0';
    }
}


uint32 ByteBuffer::readUntil(String& buf, const char& c)
{
    uint32 start_pos = _pos;
    int32 pos = find(c);
    
    if (pos >= 0)
    {
        uint32 end_pos = static_cast<uint32>(pos) + 1;
        uint32 len = end_pos - start_pos;

        buf.copy((char*)&_buffer[start_pos], len);
        // now update the byte buffer to show we have read the data
        _pos = end_pos;
        return len;
    }

    buf.clear();
    return 0;
}

uint32 ByteBuffer::readUntil(String& buf, const String& str)
{
    uint32 start_pos = _pos;
    int32 pos = find(str);
    
    if (pos >= 0)
    {
        uint32 end_pos = static_cast<uint32>(pos) + str.length();
        uint32 len = end_pos - start_pos;
        buf.copy((char*)&_buffer[start_pos], len);
        // now update the byte buffer to show we have read the data
        _pos = end_pos;
        return len;
    }

    buf.resize(0);
    return 0;
}

int32 ByteBuffer::find(const SRL::byte& value)
{
    for (uint32 i=_pos; i < _length; ++i)
    {
        if (_buffer[i] == value)
            return i;
    }
    return -1;
}

int32 ByteBuffer::find(const String& value)
{
    uint32 p = 0;
    for (uint32 i=_pos; i < _length; ++i)
    {
        p = 0;
        while (_buffer[i+p] == value[p])
        {
            ++p;
            if (p == value.length())
                return i;
        }
    }
    return -1;
}

//! Tests two XmlRpcValues for equality
bool ByteBuffer::operator==(const ByteBuffer& other) const
{
    // if they are not even the same length then don't bother
    if (_length != other._length)
        return false;
    // compare the bytes based on the longest length (not sure about overruns then)
    return memcmp (_buffer, other._buffer, _length > other._length ? _length : other._length ) == 0;    
}

//! Tests two XmlRpcValues for inequality
bool ByteBuffer::operator!=(const ByteBuffer& other) const
{
    return !(*this == other);
}


int ByteBuffer::serialize(BinaryWriter *writer) const
{
    int counter = 0;
    // first write the length of the buffer
    counter = writer->write(&_length);
    // now write the byte array to the buffer
    counter += writer->write(_buffer, _length);
    return counter;
}

int ByteBuffer::deserialize(BinaryReader *reader)
{
    int counter = 0;
    // first get the length of the buffer
    counter = reader->read(&_length);
    // now resize the buffer so we can fit what needs to be read in
    resize(_length);
    // now read in the buffer
    counter += reader->read(_buffer, _length);
    return counter;
}

bool ByteBuffer::atEnd() const
{
    if (_pos >= _length)
        return true;
    return false;
}

bool ByteBuffer::canRead(int msec) const
{
    // we can ignore msec here
    return atEnd();
}

SRL::byte ByteBuffer::read()
{
    return _buffer[_pos++];
}

bool ByteBuffer::canWrite(int msec) const
{
    return (_pos < _size);
}


int ByteBuffer::writeData(const void* obj, uint32 objsize, uint32 arrysize)
{
    // make sure we have room for the object
    uint32 write_size = objsize*arrysize;
    if ((write_size + _pos) >= _size)
        resize(write_size + _pos);
    
    ::memcpy(&_buffer[_pos], obj, write_size);
    _pos += write_size;
    _length = _pos;
    _buffer[_pos] = '\0';
    return write_size;
}

int ByteBuffer::read(String &str, uint32 arrysize)
{
    int read_count = 0;
	uint32 strsize;
	if (arrysize == 0)
		read_count = read((void*)&strsize, sizeof(uint32), 1);
	else
		strsize = arrysize;

	if (strsize > 0)
	{
		str.resize(strsize);
		read_count += read((void*)str._strPtr(), sizeof(char), strsize);
		str[strsize] = '\0';
	}
	else
	{
		str.clear();
	}
	return read_count;
}

int ByteBuffer::read(void* obj, uint32 objsize, uint32 arrysize)
{
    uint32 read_size = objsize*arrysize;
    if ((read_size + _pos) >= _length)
    {
        throw IndexOutOfBoundsException(String::Format("attempting to read past the byte buffer boundry [pos=%u, len=%u, read=%u]", 
                        _pos, _length, read_size));
    }
    ::memcpy(obj, &_buffer[_pos], objsize*arrysize);
    _pos += read_size;
    return read_size;
}


void ByteBuffer::close()
{
    resize(0);
}
