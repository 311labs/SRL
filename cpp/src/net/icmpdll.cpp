
#include "srl/net/icmpdll.h"
#ifdef WIN32

using namespace SRL;
using namespace SRL::Net;
using namespace SRL::Net::ICMP;

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501 
#include "windows.h"

#if _MSC_VER > 1000
#pragma once
#endif

struct IP_OPTS
{
	SRL::byte ttl;		// time to live
	SRL::byte tos;		// type of service
	SRL::byte ip_flags;	// IP flags
	SRL::byte opts_size;	// size of options data buffer
	SRL::byte *options;	// options data buffer
};

struct ICMP_ECHO
{
	uint32 source;		// source ip address
	uint32 status;		// ip status
	uint32 rtt;			// round trip time
	uint16 data_size;	// reply data size
	uint16 reserved;	// unknown field
	void *data;			// reply data buffer
	IP_OPTS ip_info;	// Reply options
};


#define IP_SUCCESS 11000
#define IP_DEST_NET_UNREACHABLE 11002
#define IP_DEST_HOST_UNREACHABLE 11003
#define IP_DEST_PROT_UNREACHABLE 11004
#define IP_DEST_PORT_UNREACHABLE 11005
#define IP_HW_ERROR 11008
#define IP_REQ_TIMED_OUT 11010
#define IP_BAD_REQ 11011
#define IP_BAD_ROUTE 11012
#define IP_TTL_EXPIRED_TRANSIT 11013
#define IP_BAD_DESTINATION 11018

extern "C" 
{	
	/** Opens a handle on which ICMP Echo Requests can be issued. */
	HANDLE WINAPI IcmpCreateFile(VOID);
	/** Closes the handle opened by IcmpCreateFile */
	BOOL WINAPI IcmpCloseHandle(HANDLE  IcmpHandle);
	/** Sends an ICMP Echo request and returns any replies. The call returns
	* when the timeout has expired or the reply buffer is filled. */
	DWORD WINAPI IcmpSendEcho(HANDLE icmp_handle, uint32 ip_addr, LPVOID req_data,
		uint16 req_size, IP_OPTS *req_opts, LPVOID reply_data, DWORD reply_size,
		DWORD timeout);
}


Net::ICMP::Response Net::ICMP::IcmpDllEchoRequest(const InetSocketAddress &addr, 
    const uint32 &seq, uint16 msg_size, uint32 timeout, uint32 ttl)
{
	// open the icmp file handle
	HANDLE icmp_handle = IcmpCreateFile();
	if (icmp_handle == INVALID_HANDLE_VALUE)
	{
		throw SRL::Errors::IOException("failed to create icmp file handle!");
	}

	IP_OPTS req_opts;

	req_opts.ttl = ttl;
	req_opts.tos = 0;
	req_opts.ip_flags = 0;
	req_opts.opts_size = 0;
	req_opts.options = NULL;
    
	ICMP_ECHO *echo_reply=NULL;

	uint32 ip_addr = *(uint32*)addr.getAddress().getAddress();
	uint32 reply_size = sizeof(ICMP_ECHO) + MAX(msg_size, 8);

	int results = 0;

	SRL::byte *buffer = new SRL::byte[reply_size];
	memset(buffer, 'x', reply_size);

    SRL::byte code = 0;

	if (msg_size > 0)
	{
		results = IcmpSendEcho(icmp_handle, ip_addr, 
						buffer, msg_size, &req_opts,
						buffer, reply_size, timeout);
	}
	else
	{
		results = IcmpSendEcho(icmp_handle, ip_addr, 
						NULL, 0, &req_opts,
						buffer, reply_size, timeout);
	}
	
	echo_reply = (ICMP_ECHO*)buffer;
	Message::Type mtype = Message::ECHO_REPLY;

    if ((results < 1) || (echo_reply->status != 0))
    {
        switch (echo_reply->status)
        {
            case (IP_SUCCESS):
                mtype = Message::ECHO_REPLY;
                break;
            case (IP_DEST_NET_UNREACHABLE):
                code = 0;
                mtype = Message::DESTINATION_UNREACHABLE;
                break;
            case (IP_DEST_HOST_UNREACHABLE):
                code = 1;
                mtype = Message::DESTINATION_UNREACHABLE;
                break;
            case (IP_DEST_PROT_UNREACHABLE):
                code = 2;
                mtype = Message::DESTINATION_UNREACHABLE;
                break;
            case (IP_DEST_PORT_UNREACHABLE):
                code = 3;
                mtype = Message::DESTINATION_UNREACHABLE;
                break;
            case (IP_HW_ERROR):
                mtype = Message::ECHO_REPLY;
                break;
            case (IP_REQ_TIMED_OUT):
                delete buffer;
                throw SRL::Net::Errors::SocketTimeoutException("echo request timed out");
            case (IP_TTL_EXPIRED_TRANSIT):
                mtype = Message::TIME_EXCEEDED;
                break;
            case (IP_BAD_DESTINATION):
                delete buffer;
                throw SRL::Net::Errors::SocketException("bad destination");               
            default:
                uint32 ip_status = echo_reply->status;
                delete buffer;
                throw SRL::Net::Errors::SocketException(String::Format("echo request failed: (%u) '%s'", ip_status,
                    SRL::System::GetLastErrorString()));
                
        }

    }

	ICMP::Response msg(seq, echo_reply->ip_info.ttl,
		echo_reply->rtt, echo_reply->source, echo_reply->data_size, 
		    mtype, code);

	// close the icmp file handle
	IcmpCloseHandle(icmp_handle);
	delete buffer;

	return msg;
}
#else

#endif


