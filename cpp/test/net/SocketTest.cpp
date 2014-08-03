#include "SocketTest.h"

#include "srl/net/Socket.h"
#include "srl/net/TcpSocket.h"
#include "srl/net/DatagramSocket.h"
#include "srl/sys/Thread.h"
#include "srl/Exceptions.h"

#define TCP_TEST_HOST "127.0.0.1"
#define TCP_TEST_PORT 10005
#define TEST_MESSAGE1 "HELLO MY NAME IS JOE SMITH."
#define TEST_MESSAGE2 "Oh Hello, I am Bob!"
#define TEST_MESSAGE3 "Sorry but I must say goodbye."
#define TEST_MESSAGE4 "It was nice to meet you!"
#define TEST_MESSAGE5 "Until Next Time#@"

using namespace SRL;
using namespace SRL::Net;
using namespace SRL::Test;

bool SocketTest::netaddressTest()
{
    InetAddress google("www.google.com");
    TEST_ASSERT(google.isValid())
    const SRL::byte* rawip = google.getAddress();
    String ip = String::Format("%u.%u.%u.%u", rawip[0], rawip[1], rawip[2], rawip[3]);
    TEST_ASSERT(google.getHostAddress() == ip)
    TEST_ASSERT(google.getHostName().endsWith("google.com"))
    
    InetAddress googlecopy;
    TEST_ASSERT(!googlecopy.isValid())
    googlecopy = google;
    TEST_ASSERT(googlecopy.isValid())
    const SRL::byte* rawip2 = google.getAddress();
    String ip2 = String::Format("%u.%u.%u.%u", rawip2[0], rawip2[1], rawip2[2], rawip2[3]);
    TEST_ASSERT(googlecopy.getHostAddress() == ip2)
    TEST_ASSERT(googlecopy.getHostName().endsWith("google.com"))    
    TEST_THROWS(new InetAddress("gg"), Net::Errors::InvalidAddressException)
    
    InetAddress local("localhost");
    TEST_ASSERT(local.isValid())
    TEST_ASSERT(local.getHostAddress() == "127.0.0.1")
    TEST_ASSERT(local.getHostName() == "localhost")
    
    
	return true;
}

bool SocketTest::sockaddressTest()
{
    String ip = "0.0.0.0";
    InetAddress all(ip);
    InetSocketAddress sockaddr1(all, 80);
    TEST_ASSERT(sockaddr1.getPort() == 80)
    TEST_ASSERT(sockaddr1.getAddress().getHostAddress() == ip)

    ip = "64.80.184.240";
    InetSocketAddress sockaddr2(ip, 8080);
    TEST_ASSERT(sockaddr2.getPort() == 8080)
    TEST_ASSERT(sockaddr2.getAddress().getHostAddress() == ip)

    sockaddr1 = sockaddr2;
    TEST_ASSERT(sockaddr1.getPort() == 8080)
    TEST_ASSERT(sockaddr1.getAddress().getHostAddress() == ip)

    InetSocketAddress sockaddr3(22);
    TEST_ASSERT(sockaddr3.getPort() == 22)
    TEST_ASSERT(sockaddr3.getAddress().getHostAddress() == "0.0.0.0")    

	return true;
}

class SimpleServer : public System::Thread
{
public:
    SimpleServer(): start_flag(false){};
    String msg;
    bool start_flag, bind_flag, msg1_flag, con1_flag, con2_flag, close_flag, aborted;
protected:
    bool run()
    {
        start_flag = true;
        bind_flag = false;
        msg1_flag = false;
        con1_flag = false;
        con2_flag = false;
        close_flag = false;
        aborted = false;
        try
        {
            // create tcp socket for server
            Socket server;
            // bind the server to the localhost and our test port
            server.bind(TCP_TEST_HOST, TCP_TEST_PORT);
            // accept two connections
            server.listen();
            bind_flag = true;
                        
            Socket con1(server.accept());
            if (con1.getLocalAddress().getPort() != TCP_TEST_PORT)
            {
                aborted = true;
                server.close();
                msg = "connection does not match";
                return false;                
            }
            
            // receive data on the connection
            String buf;
            con1.recv(buf, 256, 500);
            if (buf != TEST_MESSAGE1)
            {
                aborted = true;
                server.close();
                msg = "con1 message does not match";
                return false;
            }
            msg1_flag = true;
            // send some data back
            con1.send(TEST_MESSAGE2);
            con1.send("Sorry but I must");
            System::Sleep(500);
            con1.send(" say goodbye.");
            con1.send(TEST_MESSAGE4);
            con1.send(TEST_MESSAGE5);
            // close the socket
            con1.close();
            con1_flag = true;
            Socket con2(server.accept());
            server.close();
        }
        catch (SRL::Errors::Exception &e)
        {
            aborted = true;
            msg = e.message();
            return false;
        }
        close_flag = true;
        return false;
    }
};

bool SocketTest::tcpSocketTest()
{
    SimpleServer server;
    server.start();
    System::Sleep(500);
    TEST_ASSERT(server.start_flag)
    TEST_ASSERT_MSG(server.bind_flag, server.msg.text())
    TEST_ASSERT_MSG(!server.aborted, server.msg.text())
    // create tcp socket
    TcpSocket client;
    client.connect(TCP_TEST_HOST, TCP_TEST_PORT);
    TEST_ASSERT(client.getRemoteAddress().getPort() == TCP_TEST_PORT);
    client.send(TEST_MESSAGE1);
    TEST_ASSERT_MSG(!server.aborted, server.msg.text())
    System::Sleep(100);
    TEST_ASSERT_MSG(!server.aborted, server.msg.text())    
    TEST_ASSERT(server.msg1_flag)
    // test if something is already running on a port
    Socket psocket;
    TEST_THROWS(psocket.bind(TCP_TEST_PORT), Net::Errors::SocketException);
    
    String in;
    client.readUntil(in, '!');
    TEST_ASSERT(in == TEST_MESSAGE2);
    client.readUntil(in, '.', 2000);
    TEST_ASSERT_MSG(in == TEST_MESSAGE3, in.text());
    client.readUntil(in, '!', 2000);
    TEST_ASSERT(in == TEST_MESSAGE4);
    client.readUntil(in, "#@", 2000);
    TEST_ASSERT(in == TEST_MESSAGE5);
    return true;
}

bool SocketTest::udpSocketTest()
{
    DatagramSocket server(TCP_TEST_PORT);
    DatagramSocket client;
    
    InetSocketAddress toaddr(TCP_TEST_PORT);
    client.sendTo(toaddr, TEST_MESSAGE1);
    InetSocketAddress fromaddr;
    char buf[256];
    int len = server.recvFrom(fromaddr, (SRL::byte*)buf, 255);
    buf[len] = '\0';
    String msg = buf;
    TEST_ASSERT(msg == TEST_MESSAGE1)
    server.close();
    return true;
}

