#include "srl/net/HttpClient.h"

using namespace SRL;
using namespace SRL::Net;

HttpClient::HttpClient() :
_socket(NULL),
_headers(Log::Logger::GetInstance("HttpClient")),
_log(Log::Logger::GetInstance("HttpClient"))
{
    
}

HttpClient::~HttpClient()
{
    if (_socket != NULL)
        delete _socket;
}

String& HttpClient::get(const URI& uri, String& output)
{
    _socket = new TcpSocket();
    uint32 port = uri.port();
    // TODO need a better port resolver 
    if (port == 0)
    {
        if (uri.scheme().isEmpty() || (uri.scheme() == "http"))
        {
            port = 80;
        }
        else if (uri.scheme() == "https")
        {
            port = 443;
        }
        else
        {
            throw SRL::Errors::IOException("invalid HTTP Scheme");
        }
    
    }
    _log.info("connecting to: %s:%d", uri.host().text(), port);
    _socket->connect(uri.host(), port);
    _log.debug("connected");
    // send the http header
    _socket->send(String::Format("GET %s HTTP/1.1\r\n", uri.path().text()));
    _socket->send(String::Format("host: %s:%d\r\n", uri.host().text(), port));
    _socket->send("accept: */*\r\n");
    _socket->send("connection: close\r\n");
    int major, minor, revision;
    SRL::GetVersion(major, minor, revision);
    String user_agent = String::Format("user-agent: SRL-HttpClient/%d.%d.%d\r\n", major, minor, revision);
    _socket->send(user_agent);
    _socket->send("\r\n");
    _log.debug("headers sent, waiting for response");
    // now get the response (wait for 30 seconds)
    String line;
    _socket->readUntil(line, '\n', 30000);
    _log.debug("response: %s", line.trim().text());
    if (line.isEmpty())
        throw SRL::Errors::IOException("failed to receive response from request");
    
    if (!line.beginsWith("HTTP"))
    {
        throw SRL::Errors::IOException("invalid response from request");
    }    
    // parse the headers
    _headers.parse(*_socket);
    if (_headers.hasKey("content-length"))
    {
        int content_length = _headers.getAsInt("content-length");
        if (content_length > 0)
        {
            _log.debug("response has content of size: %d", content_length);
            _socket->readAll(output, content_length);
        }
    }
    return output;
}