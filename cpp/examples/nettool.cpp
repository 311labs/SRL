
#include "srl/app/ArgumentParser.h"
#include "srl/Console.h"
#include "srl/net/NetworkInterface.h"

#include "srl/net/IcmpSocket.h"
#include "srl/net/DatagramSocket.h"
#include "srl/sys/Thread.h"
#include "srl/net/icmpdll.h"
#include "srl/net/HttpClient.h"

using namespace SRL;
using namespace SRL::Net;

void show_netio(bool repeat)
{
	do
	{
		NetworkInterfaces* iface_list = NetworkInterface::GetAll();
		if ((repeat)&&(iface_list->size() > 0))
		{
			System::Sleep(500);
			Console::clear();
		}

		Console::formatLine("%d interfaces", iface_list->size());

		for (int i=0; i<iface_list->size();++i)
		{
			NetworkInterface* iface = (*iface_list)[i];
			String hd_type = "Ethernet";
			if (iface->isLoopback())
				hd_type = "Loopback";
			else if (iface->isWifi())
				hd_type = "Wireless";
				
			int bytes = iface->rx().bytes();
			Console::formatLine("\n%s\tLink encap: %s\tMac: %s", iface->name().text(), hd_type.text(), iface->mac().text());
			
			if (iface->isWifi())
			{
				Console::formatLine("\tESSID: %s\tAP Mac: %s", iface->essid().text(), iface->apMac().text());
			}
			
			Console::format("\tInet Addr: %s", iface->ip().text());

			Console::format("\tBroadcast: %s",  iface->broadcast().text());
			Console::format("\tNetmask: %s\n",  iface->netmask().text());
			
            Console::format("\tmtu: %d\tmetric: %d\tspeed: %d\n", iface->mtu(), iface->metric(), iface->speed());
			
			Console::format("\trx:  ");
			Console::format("Bytes: %u",  iface->rx().bytes());
			Console::format("\tPackets: %u",  iface->rx().packets());
			Console::format("\terrors: %u",  iface->rx().errors());
			Console::format("\tDrops: %u",  iface->rx().dropped());
			Console::format("\tMulticast: %u\n",  iface->rx().multicast());

			Console::format("\ttx:  ");
			Console::format("Bytes: %u",  iface->tx().bytes());
			Console::format("\tPackets: %u",  iface->tx().packets());
			Console::format("\terrors: %u",  iface->tx().errors());
			Console::format("\tDrops: %u\n",  iface->tx().dropped());	
	
            Console::format("\tcollisions: %d\n", iface->collisions());
	
		}
	}
	while (repeat);
}

void printResult(ICMP::Message &result)
{
	switch (result.type())
	{
		case (ICMP::Message::ECHO_REPLY):
			Console::formatLine("%d bytes from %s | icmp_seq=%d | time=%0.4f ms | ttl=%d",
				result.size(), result.from().getAddress().getHostAddress().text(),
				result.sequence(), result.roundTripTime(), result.timeToLive());
			break;
		case (ICMP::Message::DESTINATION_UNREACHABLE):
		{
            String details = "Destination Unreachable";
            switch (result.code())
            {
                case (0):
                    details = "network unreachable";
                    break;
                case (1):
                    details = "host unreachable";
                    break;
                case (2):
                    details = "protocol unreachable";
                    break;
                case (3):
                    details = "port unreachable";
                    break;
                case (7):
                    details = "unknown host";
                    break;

            }
			Console::formatLine("from %s | icmp_seq=%d | %s",
				result.from().getAddress().getHostAddress().text(), result.sequence(), details.text());
			break;
		}
		case (ICMP::Message::TIME_EXCEEDED):
		Console::formatLine("%d bytes from %s | icmp_seq=%d | time=%0.4f ms | ttl=%d | TIME EXCEEDED",
			result.size(), result.from().getAddress().getHostAddress().text(),
			result.sequence(), result.roundTripTime(), result.timeToLive());
            break;
		default:
			Console::formatLine("response type not handled: %d", (int)result.type());
	}

}

void ping(SRL::String ip, int count, int size)
{
    // FIXME... this will fail because it needs to check the uid for the message
    // which is done in the receiveEchoReply... not the recvFrom
	try
	{
        ICMP::Response reply;
		ICMP::IcmpSocket icmp_socket;
		Console::formatLine("Sending %d bytes to %s", size, ip.text());
		for (uint16 seq=0; seq < count; ++seq)
		{
			System::Sleep(500);
			icmp_socket.sendEchoRequest(ip, seq, size);	
			reply = icmp_socket.receiveEchoReply(5000);		    
            printResult(reply);
	    }
	}
	catch(SRL::Errors::Exception &e)
	{
		Console::err::writeLine(e.message());
	}
}

void pingOld(SRL::String ip, int count, int size)
{
    // FIXME... this will fail because it needs to check the uid for the message
    // which is done in the receiveEchoReply... not the recvFrom
	try
	{
		ICMP::RawMessage msg(1024);
		ICMP::IcmpSocket icmp_socket;
		Console::formatLine("Sending %d bytes to %s", size, ip.text());
		for (uint16 seq=0; seq < count; ++seq)
		{
			System::Sleep(500);
			icmp_socket.sendEchoRequest(ip, seq, size);		    
            if (icmp_socket.recvFrom(msg, 5000))
            {
                printResult(msg);
            }
            else
            {
                Console::writeLine("timed out");
            }
	    }
	}
	catch(SRL::Errors::Exception &e)
	{
		Console::err::writeLine(e.message());
	}
}

void traceroute(SRL::String host)
{
    ICMP::IcmpSocket icmp_socket;
    InetSocketAddress to_addr(host);
	String ip = to_addr.getAddress().getHostAddress();
	ICMP::Response reply;
    uint32 size = 56;
    uint32 count = 30;
    uint32 seq = 0;
	Console::formatLine("traceroute to %s(%s), 30 hops max,  %d bytes",  host.text(), to_addr.getAddress().getHostAddress().text(), size);

	for (uint16 seq=0; seq < count; ++seq)
	{
		System::Sleep(10);
		Console::format("  %d", seq+1);
		String hopip = "unknown";
		for (uint32 a=0; a<3;++a)
		{
			try
			{
			    icmp_socket.sendEchoRequest(ip, seq, size, seq+1);
                reply = icmp_socket.receiveEchoReply(5000);
				Console::format("\t%0.2f", reply.roundTripTime());
				// fixme
				if (hopip == "unknown")
					hopip = reply.from().getAddress().getHostAddress().text();
			}				
			catch(SRL::Net::Errors::SocketTimeoutException &e)
			{
				Console::write("\t*");
			}
		}
		Console::write("\t");
		Console::writeLine(hopip);
		if (hopip == ip)
			break;
	}
}


void tracerouteOld(SRL::String ip)
{
	try
	{
		InetSocketAddress to_addr(ip, 33435);
		ICMP::RawMessage msg(1024);
		ICMP::IcmpSocket icmp_socket;
        DatagramSocket udp_socket;
        uint32 size = 56;
        uint32 count = 2;
        uint32 seq = 0;
		Console::formatLine("traceroute to %s(%s), 30 hops max,  %d bytes",  ip.text(), to_addr.getAddress().getHostAddress().text(), size);
		for (uint16 hop=1; hop < 25; ++hop)
		{
		    // udp or icmp?
            float64 timer = System::GetSystemTimer();
            // udp_socket.setTimeToLive(hop);
            // udp_socket.sendTo(to_addr, (SRL::byte*)&timer, sizeof(timer));
            // udp_socket.sendTo(to_addr, (SRL::byte*)&timer, sizeof(timer));
            
            icmp_socket.getSocket().setTimeToLive(hop);
            icmp_socket.sendEchoRequest(ip, hop, size, hop);
            //             
            Console::format("%2d", hop);
            
            if (icmp_socket.recvFrom(msg, 5000))
            {
                Console::format(" %s(%s)", msg.from().getHostName().text(), 
                    msg.from().getAddress().getHostAddress().text());
                Console::format("\t%1.2f", msg.roundTripTime());
                Console::format(" %d ", msg.type());
            }
            else
            {
                Console::write("\t*");
            }
            
			icmp_socket.sendEchoRequest(ip, hop, size, hop);
            if (icmp_socket.recvFrom(msg, 5000))
            {
                Console::formatLine("\t%1.2f", msg.roundTripTime());
            }
            else
            {
                Console::writeLine("\t*");
            }            
            
            if (msg.type() == ICMP::Message::DESTINATION_UNREACHABLE)
                break;
            
			System::Sleep(500);

		}
	}
	catch(SRL::Errors::Exception &e)
	{
		Console::err::writeLine(e.message());
	}
}

void listenICMP()
{
    ICMP::IcmpSocket icmp_socket;
    ICMP::RawMessage msg(1024);
	while (true)
	{
	    if (icmp_socket.recvFrom(msg, -1))
        {
            Console::formatLine("icmp packet(%d)", msg.type());
        }
    }
    
}


#ifdef WIN32
void tracerouteICMP(SRL::String host)
{
	try
	{
		InetSocketAddress to_addr(host);
		String ip = to_addr.getAddress().getHostAddress();
		ICMP::Response reply;
        uint32 size = 56;
        uint32 count = 30;
        uint32 seq = 0;
		Console::formatLine("traceroute to %s(%s), 30 hops max,  %d bytes",  host.text(), to_addr.getAddress().getHostAddress().text(), size);

		for (uint16 seq=0; seq < count; ++seq)
		{
			System::Sleep(10);
			Console::format("  %d", seq+1);
			String hopip = "unknown";
			for (uint32 a=0; a<3;++a)
			{
				try
				{
					reply = Net::ICMP::IcmpDllEchoRequest(ip, seq, size, 5000, seq+1);
					Console::format("\t%0.2f", reply.roundTripTime());
					// fixme
					if (a == 0)
						hopip = reply.from().getAddress().getHostAddress().text();
				}				
				catch(SRL::Net::Errors::SocketTimeoutException &e)
				{
					Console::write("\t*");
				}
			}
			Console::write("\t");
			Console::writeLine(hopip);
			if (hopip == ip)
				break;
		}

	}
	catch(SRL::Errors::Exception &e)
	{
		Console::err::writeLine(e.message());
	}
}



void icmpdll(SRL::String ip, int count, int size)
{
	try
	{
		InetSocketAddress to_addr(ip);
		ICMP::Response reply;
		Console::formatLine("Sending %d bytes to %s", size, ip.text());
		for (uint16 seq=0; seq < count; ++seq)
		{
			try
			{
				System::Sleep(500);
				reply = Net::ICMP::IcmpDllEchoRequest(ip, seq, size);
				printResult(reply);
			}
			catch(SRL::Net::Errors::SocketException &e)
			{
				Console::err::writeLine(e.message());
			}
		}

	}
	catch(SRL::Errors::Exception &e)
	{
		Console::err::writeLine(e.message());
	}
}
#endif


void get(const SRL::String& arg, bool verbose)
{
    
    if (verbose)
    {
        Log::Logger::SetGlobalPriorityLevel(Log::Logger::ALL);
    }
    
    URI uri(arg);
    HttpClient client;
    String output(512);
    try
    {
        client.get(uri, output);
    }
    catch (SRL::Errors::Exception &e)
    {
        Console::writeLine(e.message());
        
    }
    Console::writeLine("-------------");
    Console::writeLine(output.text());
    Console::writeLine("-------------");
    
}


int main (int argc, char const* argv[])
{
    App::ArgumentParser argparser("SRL Network Tool", "This is an example of how to use SRL Net Logic", "0.0.0");
    argparser.add("verbose", "-v, --verbose", "display verbose output");
    argparser.add("help", "--help", "display this output");
	argparser.add("repeat", "-r, --repeat", "repeat the command over and over");
    argparser.add("get", "-g, --get", "performs and http request to stdout", "http://localhost");
    argparser.add("ping", "-p, --ping", "ping a host", "localhost");
    argparser.add("trace", "-t, --trace", "trace a host", "localhost");
	argparser.add("io", "-i, --io", "show current io for the different interfaces");
    argparser.add("count", "-c, --count", "number of request to send out", "5");
    argparser.add("size", "-s, --size", "size in bytes of each request to send", "32");
    argparser.add("listen", "--listen", "listen for incoming data [tcp, udp, multicast, icmp]", "icmp");
#ifdef WIN32
    argparser.add("icmpdll", "--icmpdll", "use the icmp dll");
#endif
    argparser.parse(argc, argv);
    
    bool verbose = argparser.get("verbose")->wasParsed();
    
    if (argparser.get("io")->wasParsed())
    {
        show_netio(argparser.get("repeat")->wasParsed());
        return 1;
    }

    if (argparser.get("ping")->wasParsed())
    {
		SRL::String ip = argparser.get("ping")->getValue();
		int count = argparser.get("count")->getIntValue();
		int size = argparser.get("size")->getIntValue();        
#ifdef WIN32
		if (argparser.get("icmpdll")->wasParsed())
			icmpdll(ip, count, size);
		else
			ping(ip, count, size);
#else
		ping(ip, count, size);
#endif
        return 1;
    }
    else if (argparser.get("trace")->wasParsed())
    {
        SRL::String ip = argparser.get("trace")->getValue();
#ifdef WIN32        
		if (argparser.get("icmpdll")->wasParsed())
			tracerouteICMP(ip);
		else
			traceroute(ip);      
#else
        traceroute(ip);
#endif
        
        return 1;
    }
    else if (argparser.get("listen")->wasParsed())
    {
        SRL::String proto = argparser.get("listen")->getValue();
        if (proto == "icmp")
        {
            listenICMP();
        }
        return 1;        
    }
    else if (argparser.get("get")->wasParsed())
    {
        get(argparser.get("get")->getValue(), verbose);
        return 1;
    }
    
    argparser.printHelp();
    return 0;
}