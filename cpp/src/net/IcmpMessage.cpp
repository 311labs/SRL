
#include "srl/net/IcmpMessage.h"
#include "srl/sys/System.h"
#include "srl/Exceptions.h"

using namespace SRL;
using namespace SRL::Net::ICMP;
using namespace SRL::Net::Errors;

RawMessage::RawMessage(Message::Type icmp_type, const uint16 &id, const uint16 &seq,
                        const uint16 &msg_size, SRL::byte code) :
Message(),
_received_at(0.0),
_data_size(0),
_checksum(),
_buffer(msg_size),
_ip_header(NULL),
_header(NULL),
_data(NULL),
_timestamp(NULL)
{
	update(icmp_type, id, seq, msg_size, code);
}

RawMessage::RawMessage(const uint16& msg_size) :
Message(),
_received_at(0.0),
_data_size(msg_size),
_checksum(),
_buffer(msg_size),
_ip_header(NULL),
_header(NULL),
_data(NULL),
_timestamp(NULL)
{
	//update(ECHO_REPLY, 0, 0, msg_size, 0);
	resize(msg_size);
}

RawMessage::RawMessage(const RawMessage &msg) :
Message()
{
	// just copy the byte buffer then move the pointers
	_buffer = msg._buffer;
	sync(msg.type() == Message::ECHO_REQUEST);
}

RawMessage::~RawMessage()
{

}

void RawMessage::resize(const uint16& msg_size)
{
	_buffer.resize(msg_size);
}

const uint16& RawMessage::size() const
{
	return _data_size;
}

void RawMessage::update(Message::Type icmp_type, const uint16 &id,
					const uint16 &seq,
					const uint16 &msg_size,SRL::byte code)
{
	// FIXME we will always assume this is outbound messages
	if (msg_size > _buffer.size())
		_buffer.resize(msg_size);

	sync();
	// 8 + 8 = 16
	// 32 + 16 = 48
	_header->type = icmp_type;
	_header->code = code;
	_header->id = id;
	_header->seq = seq;

	_buffer.setLength(msg_size);
}

void RawMessage::update(const uint16 &id, const uint16 &sequence, const uint16 &data_size)
{
	update(Message::ECHO_REQUEST, id, sequence, data_size + ICMP::HEADER_SIZE, 0);
	
	*_timestamp = System::GetSystemTimer();
	_header->checksum = 0;

	// minus out the size of the timestamp field
	fillData(data_size);
	doChecksum();
}

void RawMessage::markReceived()
{
	this->_received_at = System::GetSystemTimer();
}

void RawMessage::sync(bool isOutbound)
{
	// first set the header
	int offset = 0;
	if (!isOutbound)
	{
		offset = IP::HEADER_SIZE;
		// point our ip header structure to the right place in the byte buffer
		_ip_header = (IP::Header*)&_buffer[0];
		
	    byte* bip = (byte*)&(_ip_header->from);
    	_from = String::Format("%d.%d.%d.%d", bip[0], bip[1], bip[2], bip[3]);		
		// set the data size field (take out the headers and add the timestamp)
		_data_size = _buffer.length() - ICMP::HEADER_SIZE - IP::HEADER_SIZE + sizeof(float64);
	}
	// point our icmp header structure to the right place in the byte buffer
	_header = (ICMP::Header*)&_buffer[offset];

	// now if we are a destination unreachable message
	// we really want to move our header to the original message header stored
	// stored in the datafield
	if (!isOutbound)
	{
        if ((_header->type == Message::TIME_EXCEEDED) || (_header->type == Message::DESTINATION_UNREACHABLE))
        {
            IP::Header *ip_header = (IP::Header*)&_buffer[IP::HEADER_SIZE + ICMP::HEADER_SIZE];

    	    byte* bip = (byte*)&(ip_header->to);
        	_from = String::Format("%d.%d.%d.%d", bip[0], bip[1], bip[2], bip[3]);
            
            
            ICMP::Header iheader;
            iheader.type = _header->type;
            iheader.code = _header->code;
            //printf("icmp info:  data size: %d\n", _data_size);  
             offset = IP::HEADER_SIZE + ICMP::HEADER_SIZE + IP::HEADER_SIZE;
             _header = (ICMP::Header*)&_buffer[offset];
             _header->type = iheader.type;
             _header->code = iheader.code;
             _timestamp = (float64*)&_buffer[offset+ICMP::HEADER_SIZE];
  			float64 time_stamp = *_timestamp;
  			_rtt = (_received_at - time_stamp) * 1000.0;
  			if (_rtt > 10000)
                  _rtt = -1.0f;
              _data_size -= 8;        
        }
        else
		if ((_header->type == Message::ECHO_REPLY))
		{
			_data = (byte*)&_buffer[offset+ICMP::HEADER_SIZE+sizeof(float64)];
			_timestamp = (float64*)&_buffer[offset+ICMP::HEADER_SIZE];
			float64 time_stamp = *_timestamp;
			_rtt = (_received_at - time_stamp) * 1000.0;
			if (_rtt > 10000)
                _rtt = 0;
			_data_size -= 8;
		}
	}
	else
	{
		// FIXME can't assume this is ECHO_REQUEST
		_data = (byte*)&_buffer[ICMP::HEADER_SIZE+sizeof(float64)];
		_timestamp = (float64*)&_buffer[ICMP::HEADER_SIZE];
	}
}

void RawMessage::doChecksum()
{
	_checksum.clear();
	_checksum.doIpCheckSum((uint16*)_buffer.bytes(), _buffer.length());
	// Calculate a checksum on the result
	_header->checksum =  _checksum.value16();
}

void RawMessage::fillData(const uint16& data_size)
{
	// add the size of the timestamp
	int fill_size = static_cast<int>(data_size) - sizeof(float64);

	if (_data_size != fill_size)
	{
		// "You're dead meat now, packet!"
		// another favorite 0xCACADEED
		// 0x474E4950474E4F50 (PINGPONG)
		const int64 deadmeat = 0x474E4F50474E4950LL;
		const SRL::byte deadnull = 0x58;
		// move the byte buffer to the position where we start to write data
		_buffer.setPosition(ICMP::HEADER_SIZE + sizeof(float64));
		int total = fill_size / 8;
		int left =  fill_size % 8;
		for (int i=0; i < total; ++i)
			_buffer.write(&deadmeat);
		// fill whats left with 0s
		for (int i=0; i < left; ++i)
			_buffer.write(&deadnull);

		_data_size = fill_size;
	}
}

Response::Response(const uint16 &rep_seq, const SRL::byte &rep_ttl, 
					const uint32 &rep_rtt, const uint32 &rep_from, 
					const uint32& rep_size, Message::Type mtype, const SRL::byte& code):
_type(mtype), _code(code),
_id(0), _seq(rep_seq), _checksum(0),
_size(rep_size), _ttl(0)
{
	byte* bip = (byte*)&rep_from;
	_from = String::Format("%d.%d.%d.%d", bip[0], bip[1], bip[2], bip[3]);

	if ((rep_rtt < 1) || (rep_rtt > 10000000))
		_rtt = 0.0f;
	else
		_rtt = (float)rep_rtt;
}

Response::Response(const RawMessage& msg)
{
	_from = msg.from().getAddress().getHostAddress();
	_id = msg.id();
	_type = msg.type();
	_code = msg.code();
	_seq = msg.sequence();

	_size = msg.size();
	_ttl = msg.timeToLive();
	_rtt = msg.roundTripTime();


}

