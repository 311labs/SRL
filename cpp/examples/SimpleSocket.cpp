
#include "srl/net/DatagramSocket.h"
#include "srl/net/MulticastSocket.h"
#include "srl/app/ArgumentParser.h"
#include "srl/Console.h"
#include "srl/DateTime.h"

using namespace SRL;
using namespace SRL::Net;


int run_mcast_server(InetSocketAddress &addr)
{
    try
    {
        InetSocketAddress from;
        SRL::byte buf[256];
        DateTime dt;
        MulticastSocket server(addr);
        Console::formatLine("Multicast Server now listening: %s:%d", 
            server.getSocket().getLocalAddress().getAddress().getHostAddress().text(),
            server.getSocket().getLocalAddress().getPort());

        Console::formatLine("Multicast Interface: %s", 
            server.getMulticastInterface().getHostAddress().text());
        
        Console::formatLine("Multicast Loopback: %d", 
            server.getLoopbackMode());

        Console::formatLine("Multicast TTL: %d", 
            server.getTimeToLive());
            
        while (1)
        {
            server.recvFrom(from, buf, 255);
            Console::formatLine("%s:%d sent: '%s'", from.getAddress().getHostAddress().text(), from.getPort(), buf);
        }
        server.close();
    }
    catch (SRL::Errors::Exception &e)
    {
        Console::writeLine(e.message());
    }
    return 0;
}

int run_mcast_client(InetSocketAddress &addr)
{
    try
    {
        InetSocketAddress from;
        SRL::byte buf[256];
        DateTime dt;
        MulticastSocket client;
        while (1)
        {
            dt.update();
            client.sendTo(addr, dt.asString());
            Console::formatLine("sent: '%s'", dt.asString().text());
            System::Sleep(1000);
        }
        client.close();
    }
    catch (SRL::Errors::Exception &e)
    {
        Console::writeLine(e.message());
    }
    return 0;
}


int run_udp_server(InetSocketAddress &addr)
{
    try
    {
        InetSocketAddress from;
        SRL::byte buf[256];
        DateTime dt;
        DatagramSocket server(addr);
        Console::formatLine("UDP Server now listening: %s:%d", 
            server.getSocket().getLocalAddress().getAddress().getHostAddress().text(),
            server.getSocket().getLocalAddress().getPort());
            
        while (1)
        {
            server.recvFrom(from, buf, 255);
            Console::formatLine("%s:%d sent: '%s'", from.getAddress().getHostAddress().text(), from.getPort(), buf);
        }
        server.close();
    }
    catch (SRL::Errors::Exception &e)
    {
        Console::writeLine(e.message());
    }
    return 0;
}

int run_udp_client(InetSocketAddress &addr)
{
    try
    {
        InetSocketAddress from;
        SRL::byte buf[256];
        DateTime dt;
        DatagramSocket client;
        while (1)
        {
            dt.update();
            client.sendTo(addr, dt.asString());
            Console::formatLine("sent: '%s'", dt.asString().text());
            System::Sleep(1000);
        }
        client.close();
    }
    catch (SRL::Errors::Exception &e)
    {
        Console::writeLine(e.message());
    }
    return 0;
}

int run_tcp_server(InetSocketAddress &addr)
{
    try
    {
        DateTime dt;
        Socket server(Socket::TCP);
        server.bind(addr);
        server.listen();

        Console::formatLine("TCP Server now listening: %s:%d", 
            server.getLocalAddress().getAddress().getHostAddress().text(),
            server.getLocalAddress().getPort());
        String buf;
        while (1)
        {
            Socket con(server.accept());
            Console::formatLine("TCP Server new connection: %s:%d - %s:%d", 
                con.getRemoteAddress().getAddress().getHostAddress().text(),
                con.getRemoteAddress().getPort(),
                con.getLocalAddress().getAddress().getHostAddress().text(),
                con.getLocalAddress().getPort());
            try
            {
                while (con.isConnected())
                {
                    dt.update();
                    con.send(dt.asString());
                    con.recv(buf, 256, 1000);
                    Console::formatLine("TCP Server received: %s", buf.text());
                    System::Sleep(1000);
                }
            }
            catch (Net::Errors::SocketException &e)
            {
                Console::formatLine("Connection disconnected: %s", e.message().text());
            }
        }
        
    }
    catch (SRL::Errors::Exception &e)
    {
        Console::writeLine("server failed with: " + e.message());
        return 0;
    }
    return 1;
}

int run_tcp_client(InetSocketAddress &addr)
{
    try
    {
        DateTime dt;
        Socket client(Socket::TCP);
        client.connect(addr);
        Console::formatLine("TCP Client now connected to: %s:%d", 
            client.getRemoteAddress().getAddress().getHostAddress().text(),
            client.getRemoteAddress().getPort());
        String buf;
        while (client.isConnected())
        {
            client.recv(buf, 256, 1000);
            Console::formatLine("TCP Client received: %s", buf.text());            
            dt.update();
            client.send(dt.asString());
            System::Sleep(1000);
        }
        
    }
    catch (SRL::Errors::Exception &e)
    {
        Console::writeLine("client failed with: " + e.message());
        return 0;
    }
    return 1;
}


int main(int argc, const char* argv[])
{
    App::ArgumentParser argparser("SRL SimpleSocket Example", "This is an example of how to use SRL Sockets", "0.0.0");
    argparser.add("verbose", "-v, --verbose", "display verbose output");
    argparser.add("help", "--help", "display this output");
    argparser.add("server", "-s, --server", "run the socket server");
    argparser.add("client", "-c, --client", "run the socket client");
    argparser.add("port", "-p, --port", "the socket port to use", "10005");
    argparser.add("host", "-h, --host", "the socket host to use", "");
    argparser.add("udp", "--udp", "set protocol to udp");
    argparser.add("multicast", "--multicast", "set protocol to multicast");
    argparser.parse(argc, argv);
    
    if (argparser.get("help")->wasParsed())
    {
        argparser.printHelp(); 
        return 1;
    }
    
    int port = argparser.get("port")->getIntValue();
    
    InetSocketAddress host_addr(port);
    if (argparser.get("host")->wasParsed())
    {
        InetSocketAddress new_addr(argparser.get("host")->getValue(), port);
        host_addr = new_addr;
    }
    
    if (argparser.get("udp")->wasParsed())
    {
        if (argparser.get("server")->wasParsed())
            return run_udp_server(host_addr);
        else
            return run_udp_client(host_addr);        
    }
    else if (argparser.get("multicast")->wasParsed())
    {
        if (argparser.get("server")->wasParsed())
            return run_mcast_server(host_addr);
        else
            return run_mcast_client(host_addr);        
    }    
    else
    {
        if (argparser.get("server")->wasParsed())
            return run_tcp_server(host_addr);
        else
            return run_tcp_client(host_addr);
    }
    argparser.printHelp();   
    return 0;
}