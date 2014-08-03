
#ifndef __SRL_ByteBuffer__
#define __SRL_ByteBuffer__

#include "srl/io/BinaryReader.h"
#include "srl/Exceptions.h"
#include "srl/io/BinaryWriter.h"


// TDDO: FIX THIS SUCKER UP.. .IT IS A MESS (LIKE NO BOUNDS CHECKCINGS), ians

namespace SRL
{
    namespace IO
    {
        /** Byte Buffer Array */
        class SRL_API ByteBuffer : public BinaryReader, public BinaryWriter
        {
        public:
            /** allocate a byte buffer of specified size */
            ByteBuffer(uint32 new_size=1024);
            /**   
             * Wrap the existing byte array with the byte buffer It is
             * assumed that if no buf_length is passed in that the entire array
             * is the length and if no size is passed in then a sizeof(buffer) is
             * called
             */
            ByteBuffer(byte* buffer, uint32 buf_size=0, uint32 buf_length=0);
    
            /** copy constructor */
            ByteBuffer(const ByteBuffer &buffer);
            
            /** destructor */
            virtual ~ByteBuffer();
    
            /** is at end of buffer */
            bool atEnd() const;
            
            /** return the byte array backing this buffer const version  */
            const char* c_str() const{ return (const char*)_buffer; }
            
            /** return the byte array backing this buffer const version  */
            const SRL::byte* bytes() const{ return _buffer; }
            /** return the raw byte array DANGER */
            byte* raw(){ return _buffer; }
    
            /** toggle the read and write modes */
            void toggle(bool write_mode=true);
    
            /**
            * @deprecated
            *  set the length to the current position and move the cursor back to the front */
            void flip();
    
            /** resize the current byte buffer */
            void resize(uint32 new_size);
    
            /** return the current size of the byte buffer (this is the actual amount of memory allocated) */
            const uint32& size() const{ return _size; }
            /** return the current length of the byte buffer (this is the total amount of data in the buffer) */
            const uint32& length() const{return _length; }
            /** returns the total number of bytes that have not been read */
            uint32 remaining() const{ return _length - _pos; }
    
            /** returns the current position in the buffer */
            const uint32& position() const{return _pos; }
            /** move our cusor to the specified position */
            void setPosition(const uint32& pos);
            /** set the length of this byte buffer */
            void setLength(const uint32& length);
            
            /** makes the current position the start of the byte buffer, 
             * This is very usefull when doing things like reading a tcp
             *  stream and reading more data then you wanted */
            void moveStart();
    
            /** returns the position of the value if found, else returns -1
            * this starts at the current position so use set postion to move the start */
            int32 find(const SRL::byte& value);
            /** returns the position of the value if found, else returns -1
            * this starts at the current position so use set postion to move the start */
            int32 find(const String& value);
                        
            /** 
            * reads until the character has been encountered and puts all read data into the buf 
            * 
            * @returns the number of bytes read
            */
            uint32 readUntil(String& buf, const char& c);
            /** 
            * reads until the string has been encountered and puts all read data into the buf 
            * 
            * @returns the number of bytes read
            */
            uint32 readUntil(String& buf, const String& str);
            /** 
            * reads until the byte has been encountered and puts all read data into the buf 
            * 
            * @returns the number of bytes read
            */
            //uint32 readUntil(SRL::byte* data, const uint32& len, const SRL::byte& c);
            /** 
            * reads until the string has been encountered and puts all read data into the buf 
            * 
            * @returns the number of bytes read
            */
            //uint32 readUntil(SRL::byte* data, const uint32& len, const String& str);
        
            //! Tests two XmlRpcValues for equality
            bool operator==(const ByteBuffer& other) const;

            //! Tests two XmlRpcValues for inequality
            bool operator!=(const ByteBuffer& other) const;
            
        // array style access
            /** Return a non-const reference to the ith byte */
            SRL::byte& operator[](uint32 i){ return _buffer[i]; }
            /** Return a const reference to the ith byte */
            const SRL::byte& operator[](uint32 i) const { return _buffer[i]; }
        // serialization
            /** write this byte buffer to the serializer */
            virtual int serialize(BinaryWriter *serializer) const;
            /** extract a byte buffer from the serializer */
            virtual int deserialize(BinaryReader *serializer);
            
            void close();

            /** is the object ready to be read from */
            bool canWrite(int msec=SRL::NOW)const;          
            /** is the object ready to be read from */
            bool canRead(int msec=SRL::NOW)const;
            /** read 1 byte from the object */
            SRL::byte read();
            /** this should be implemented */
            int read(String &str, uint32 arrysize=0);


    /** write bytes to the file*/
    virtual int writeData(const void* obj, uint32 objsize, uint32 arrysize=1);
    /** read bytes from the file */
    virtual int read(void* obj, uint32 objsize, uint32 arrysize=1);
    
        protected:
            mutable SRL::byte *_buffer;
            uint32 _size;
            // TODO keep the real size in a seprate variable
            //uint32 _real_size;
            uint32 _length;
            uint32 _pos;

        };
    }
}

#endif // __SRL_ByteBuffer__

