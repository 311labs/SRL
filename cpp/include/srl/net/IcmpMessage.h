
#ifndef __SRL_IcmpMessage__
#define __SRL_IcmpMessage__

#include "srl/net/IcmpSocket.h"
#include "srl/util/CheckSum.h"
#include "srl/io/ByteBuffer.h"

namespace SRL
{
    namespace Net
    {
        namespace IP
        {
            /** standard ipv4 header */
            struct Header
            {
                SRL::byte ip_vhl;         /* Version and Header Length */
                SRL::byte tos;         /* Type of Service byte */
                SRL::uint16 len;         /* Total length of datagram */
                SRL::uint16 id;          /* Identification of datagram */
                SRL::uint16 offset;         /* Fragmentation offset */
                SRL::byte ttl;         /* Time to live byte */
                SRL::byte protocol;           /* Protocol of user level datagram */
                SRL::uint16 checksum;         /* Header checksum */
                SRL::uint32 from;         /* Source address of datagram */
                SRL::uint32 to;         /* Destination address of datagram */
            };
            const uint16 HEADER_SIZE = 20;
        }
    
        namespace ICMP
        {
            class IcmpSocket;
            /** 8 byte icmp header */
            struct Header
            {
                SRL::byte type;
                SRL::byte code;
                SRL::uint16 checksum;
                SRL::uint16 id;
                SRL::uint16 seq;
            };
            const uint16 HEADER_SIZE = 8;
        
            /** Abstract Base Class */
            class SRL_API Message
            {
            public:
                /** ICMP Message Type  */
                enum Type
                {
                    ECHO_REPLY,
                    RESERVED_1,
                    RESERVED_2,
                    DESTINATION_UNREACHABLE,
                    SOURCE_QUENCH,
                    REDIRECT_MESSAGE,
                    ALTERNATE_HOST_ADDRESS,
                    RESERVED_3,
                    ECHO_REQUEST,
                    ROUTER_ADVERTISEMENT,
                    ROUTER_SOLICITATION,
                    TIME_EXCEEDED,
                    PARAMETER_PROBLEM,
                    TIMESTAMP,
                    TIMESTAMP_REPLY,
                    INFORMATION_REQUEST,
                    INFORMATION_REPLY,
                    ADDRESS_MASK_REQUEST,
                    ADDRESS_MASK_REPLY,
                    RESERVED_FOR_SECURITY,
                    TRACEROUTE=30,
                    DATAGRAM_CONVERSION_ERROR,
                    MOBILE_HOST_REDIRECT,
                    IPV6_WHERE_ARE_YOU,
                    IPV6_HERE_I_AM,
                    MOBILE_REGISTRATION_REQUEST,
                    MOBILE_REGISTRATION_REPLY,
                    DOMAIN_NAME_REQUEST,
                    DOMAIN_NAME_REPLY
                };
                Message(){};
                virtual ~Message(){};  
                /** returns the type of message */
                virtual const Message::Type type() const=0;
                /** returns the message code (differs based on type) */
                virtual const byte& code() const=0;
                /** returns the ID (normally set by the application that sent the origninal message) */
                virtual const uint16& id() const=0;
                /** returns the sequence number */
                virtual const uint16& sequence() const=0;
                /** returns the checksum value for the entire message */
                virtual const uint16& checksum() const=0;
                /** returns a pointer to the data field of the message */
                virtual const byte* data() const=0;
                /** returns the size of the data field for the message */
                virtual const uint16& size() const=0;
                /** returns the time to live field from the ip header */
                virtual const byte& timeToLive() const=0;

                /** returns the address from the message sender */
                const InetSocketAddress& from() const{ return _from; }
                /** returns the round trip time for the message */
                const float32& roundTripTime() const{ return _rtt; }
            protected:
                /** the from address field */
                InetSocketAddress _from;
                /** holds the round trip time */
                float32 _rtt;

            };

            /** Base Class for ICMP Messages */
            class SRL_API RawMessage : public Message
            {
            friend class IcmpSocket;
            public:

            public:         
                /** constructor used by message builder */
                RawMessage(Message::Type icmp_type, const uint16 &id, const uint16 &sequence,
                            const uint16 &msg_size, byte code);
                /** constructor used for incoming messages */
                RawMessage(const uint16& msg_size);
                /** copy constructor */
                RawMessage(const RawMessage &message);
                /** destructor */
                ~RawMessage();
                /** returns the type of message */
                const Message::Type type() const{ return (Message::Type)_header->type;}
                /** returns the message code (differs based on type) */
                const byte& code() const{ return _header->code;};
                /** returns the ID (normally set by the application that sent the origninal message) */
                const uint16& id() const{ return _header->id;};
                /** returns the sequence number */
                const uint16& sequence() const{ return _header->seq;};
                /** returns the checksum value for the entire message */
                const uint16& checksum() const{ return _header->checksum;};
                /** returns a pointer to the data field of the message */
                const byte* data() const{ return _data; }
                /** returns the size of the data field for the message */
                const uint16& size() const;
                /** returns the time to live field from the ip header */
                const byte& timeToLive() const{ return _ip_header->ttl; }

            protected:
                /** reuse the current packet */
                void update(Message::Type icmp_type, const uint16 &id, const uint16 &sequence,
                            const uint16 &data_size, byte code);
                /** update used for Echo Requests */
                void update(const uint16 &id, const uint16 &sequence, const uint16 &data_size);
                /** resize the message buffer */
                void resize(const uint16& msg_size);
                
                /** check sum the entired message before sending */
                void doChecksum();
                /** fill the data field with 0xdeadbeef */
                void fillData(const uint16& data_size);

                /** sync the field pointers to the buffer */
                void sync(bool isOutbound=true);

                /** mark the message received */
                void markReceived();

                /** holds the time the message was received */
                float64 _received_at;
                
                /** size of our data (payload) */
                uint16 _data_size;
                /** our checksum */
                Util::CheckSum _checksum;
                /** byte buffer that backs this message */
                mutable IO::ByteBuffer _buffer;

                /** pointer to the ip header portion of the message */
                IP::Header *_ip_header;
                /** pointer to the icmp header */
                ICMP::Header *_header;
                /** pointer to the begining of the data field */
                byte* _data;
                /** pointer to the timestamp field */
                float64* _timestamp;
            };


            /** EchoReply Message (this can be a ECHO_REPLY or a DESTINATION_UNREACHABLE)
            * This is a copy of the original ICMP::Message and does not contain all the fields
            * like the data field
             */
            class SRL_API Response : public Message
            {
            public:
                /** simple constructor */
                Response(){}
                /** IcmpResponse constructor */
                Response(const uint16 &rep_seq, const byte &rep_ttl, 
                    const uint32 &rep_rtt, const uint32 &rep_from, 
                    const uint32& rep_size, Message::Type mtype, const SRL::byte& code);
                /** copy constructor for ICMP::Message type */
                Response(const ICMP::RawMessage &msg);
                /** returns the type of message */
                const Message::Type type() const{ return _type;}
                /** returns the message code (differs based on type) */
                const byte& code() const{ return _code;};
                /** returns the ID (normally set by the application that sent the origninal message) */
                const uint16& id() const{ return _id;};
                /** returns the sequence number */
                const uint16& sequence() const{ return _seq;};
                /** returns the checksum value for the entire message */
                const uint16& checksum() const{ return _checksum;};
                /** returns the size of the data field for the message */
                const uint16& size() const{ return _size; }

                /** returns a pointer to the data field of the message */
                virtual const byte* data() const{ return NULL;};

                /** returns the address from the message sender */
                const InetSocketAddress& from() const{ return _from; }
                /** returns the time to live field from the ip header */
                const byte& timeToLive() const{ return _ttl; }
                /** returns the round trip time for the message */
                const float32& roundTripTime() const{ return _rtt; }
                
            protected:
                Message::Type _type;
                byte _code;
                uint16 _id;
                uint16 _seq;
                uint16 _checksum;
                uint16 _size;
                byte _ttl;
            };
        }
    }
}

#endif // __SRL_IcmpMessage__
