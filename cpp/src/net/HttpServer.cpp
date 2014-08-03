#include "srl/net/HttpServer.h"
#include "srl/Exceptions.h"
#include "srl/sys/SystemInfo.h"
#include "srl/fs/Directory.h"
#include "srl/fs/TextFile.h"
#include "srl/Exceptions.h"

using namespace SRL;
using namespace SRL::Net;
using namespace SRL::System;

// === HTTP Request Class ===

HttpRequest::HttpRequest(System::ThreadManager* manager, HttpRequestHandler &handler) :
Thread(manager),
_keep_alive(false),
_content(512),
_content_length(0),
_socket(NULL),
_headers(handler.getLogger()),
_method(), _uri(), _request_version(),
_major(0),
_minor(0),
_handler(handler),
_date(),
_log(handler.getLogger()), 
_log_prefix()
{

}

HttpRequest::~HttpRequest()
{    
}

void HttpRequest::accept(const Socket::Descriptor& desc)
{
    if (_socket != NULL)
        delete _socket;
    
    _socket = new TcpSocket(desc);
    _log_prefix = String::Format("request(%s) ", _socket->getRemoteAddress().asString().text());;
}
           
/** returns the content as a string, will parse the request */
const String& HttpRequest::getContent()
{
    if (_content.isEmpty() && _content_length > 0)
        _socket->readAll(_content, _content_length);
    return _content;
}

/** returns the inetsocket address object */
const InetSocketAddress& HttpRequest::remoteAddress()
{
    return _socket->getRemoteAddress();
}

/** returns true if the header exists in this request */
bool HttpRequest::hasHeader(const String& header) const
{
    return _headers.hasKey(header);
} 
/** returns a dictionay containing the http headers included with the request */
const String& HttpRequest::getHeader(const String& header)
{
    return _headers.get(header);
}


/** 
* start sending the response header
* sends the first line of the response: 
* REQUEST_VERSION STATUS_CODE REASON
*/
void HttpRequest::startHeader(const uint32& code, const String& reason)
{
    // HTTP/0.9 supports no headers
    if ((_major != 0) && (_minor != 9))
    {
        uint32 r_major = _handler.protocolMajor();
        uint32 r_minor = _handler.protocolMinor();
        if ((_major < r_major) ||  ((_major == r_major) && (_minor < r_minor)))
        {
            r_major = _major;
            r_minor = _minor;
        }
        
        _socket->send(String::Format("HTTP/%u.%u %u %s\r\n", r_major, r_minor, code, reason.text()));
        if (!_keep_alive)
            _socket->send("Connection: close\r\n");
        
        _socket->send(String::Format("Server: %s\r\n", _handler.serverVersion().text()));
        _date.update(false);
        _socket->send(_date.formatString("Date: %a, %d %b %Y %H:%M:%S GMT\r\n"));
    }
}

/** send a response header */
void HttpRequest::sendHeader(const String& key, const String& value)
{
    // HTTP/0.9 supports no headers
    if ((_major != 0) && (_minor != 9))
    {
        _socket->send(String::Format("%s: %s\r\n", key.text(), value.text()));
    }
}

/** end the RESPONSE headers by sending the blank lines */
void HttpRequest::endHeader()
{
    // HTTP/0.9 supports no headers
    if ((_major != 0) && (_minor != 9))
        _socket->send("\r\n");
}

/** simple way to send a response back assumes no headers have been sent */
void HttpRequest::sendResponse(const String& content, const String& content_type)
{
    _log.debug("sending response");
    startHeader(200, "OK");
    sendHeader("Content-Type", content_type);
    sendHeader("Content-Length", String::Val(content.length()));
    endHeader();
    if (_socket->send(content) < content.length())
        _log.warn("failed to send all response data");
    else
        _log.debug("response sent");
	_socket->send("\r\n\r\n", 4);
}

void HttpRequest::sendError(const uint32& code, const String& reason)
{
    _log.warn("%s %u: %s", _log_prefix.text(), code, reason.text());
    startHeader(code, reason);
    sendHeader("Content-Type", "text/html");
    
    String extended_reason = "<h1>";
    extended_reason += reason;
    extended_reason += "</h1>\n";
    
    sendHeader("Content-Length", String::Val(extended_reason.length()));
    endHeader();
    _socket->send(extended_reason);    
}

/** close the connection */
void HttpRequest::close()
{
    if (_socket != NULL)
    {
        _socket->close();
    }
}

const String& HttpRequest::readLine()
{
    // lets try and read a line from the buffer and wait no more then 5 seconds
    if (_socket->readUntil(_content, '\n', 10000) <= 0)
        _content.clear(); // no data was read so lets return empty string
    return _content;
}

void HttpRequest::_parseHeaders()
{
    _content_length = 0;
    _headers.parse(*_socket);
    // check for content headers
    if (_headers.hasKey("content-length"))
    {
        _content_length = _headers.getAsInt("content-length");
    }
    _content.clear();
}

void HttpRequest::_parseVersion()
{
    if (!_request_version.isEmpty())
    {
        if (!_request_version.beginsWith("HTTP/"))
        {
            SRL::Errors::IOException err(String::Format("Bad Request Version: %s", _request_version.text()));
            sendError(400, err.message());
            throw err;
        }
        
        _major = String::Int(_request_version.between('/', '.'));
        _minor = String::Int(_request_version.between('.', '\r'));
        if ((_major == 0) && (_minor == 0))
        {
            SRL::Errors::IOException err(String::Format("Bad Request Version: %s", _request_version.text()));
            sendError(400, err.message());
            throw err;
        }
        _request_version.trimEnd();
    }
    else if (_method == "GET")
    {
        // assume we are looking at a HTTP/0.9 and it only supports GET
        _request_version = "HTTP/0.9";
        _major = 0;
        _minor = 9;
    }    
    
    _log.debug("%s http version: %d.%d", _log_prefix.text(), _major, _minor);
}

void HttpRequest::_parseRequest()
{
    // first clear the old headers
    _headers.clear();
    
    // the first line of the request should be the method URI and protocol
    // readline just updates the _content string
    readLine();
    if (_content.isEmpty())
        throw SRL::Errors::IOException("timed out waiting for headeers\n");
    _content.trimEnd();
    _method = _content.section(' ', 0);
    _uri = _content.section(' ', 1);
    _request_version = _content.section(' ', 2);
    _log.info("new %s method='%s' uri='%s'", _log_prefix.text(), _method.text(),
                _uri.text());    
    _parseVersion();
    
    // TODO decide to do keep alive or not
    
    if (_major > 0)
    {
        _parseHeaders();
    }
    
    if ((_major == 1) && (_minor >= 1))
    {
        // we must have a host header
        if (!_headers.hasKey("host"))
        {
            SRL::Errors::IOException err("Bad Request: No Host Header");
            sendError(400, err.message());
            throw err;
        }
    }
    
    if (_headers.hasKey("content-length"))
        _content_length = _headers.getAsInt("content-length");
    else
        _content_length = 0;
    
    bool is_handled = false;
    // now its time for someone to handle the request
    if (_method == "HEAD")
    {
        is_handled = _handler.http_head(*this);
    }
    else if (_method == "GET")
    {
        is_handled = _handler.http_get(*this);
    }
    else if (_method == "POST")
    {
        is_handled = _handler.http_post(*this);
    }
    else if (_method == "PUT")
    {
        is_handled = _handler.http_put(*this);
    }
    else if (_method == "DELETE")
    {
        is_handled = _handler.http_delete(*this);
    }
    else if (_method == "TRACE")
    {
        is_handled = _handler.http_trace(*this);
    }
    else if (_method == "OPTIONS")
    {
        is_handled = _handler.http_options(*this);
    }
    else
    {
        is_handled = _handler.http_unknown(*this);
    }
    
    if (!is_handled)
    {
        // we assume that the method is not allowed for this server
        sendError(400, "METHOD NOT SUPPORTED");
    }
    
    if (!_keep_alive)
    {
        close();
    }
}

bool HttpRequest::run()
{     
    try
    {
        while ((_socket != NULL)&&(_socket->isConnected()))
        {
            _parseRequest();
        }
    }
    catch(SRL::Errors::Exception &e)
    {
        _log.error(e);
    }
    return false;
}

void HttpRequest::final()
{
    close();
    _log.info("%s closed", _log_prefix.text());
    if (_socket != NULL)
    {
        delete _socket;
        _socket = NULL;
    }
}

// === HTTP Request Handler Base Class ===
HttpRequestHandler::HttpRequestHandler(uint32 protocol_major, uint32 protocol_minor) :
_major(protocol_major),
_minor(protocol_minor),
_log(Log::Logger::GetInstance("SRL HTTPD"))
{
    int major, minor, revision;
    System::SystemInfo *sysinfo = System::SystemInfo::GetInstance();
    SRL::GetVersion(major, minor, revision);
    _server_version = String::Format("SRL-HTTPD/%d.%d.%d (%s)", major, minor,
                revision, sysinfo->os().kernel().text());
    _protocol_version = String::Format("HTTP/%u.%u", protocol_major, protocol_minor);
    
}

HttpRequestHandler::HttpRequestHandler(Log::Logger &logger, uint32 protocol_major, uint32 protocol_minor) :
_major(protocol_major),
_minor(protocol_minor),
_log(logger)
{
    int major, minor, revision;
    System::SystemInfo *sysinfo = System::SystemInfo::GetInstance();
    SRL::GetVersion(major, minor, revision);
    _server_version = String::Format("SRL-HTTPD/%d.%d.%d (%s)", major, minor,
                revision, sysinfo->os().kernel().text());
    _protocol_version = String::Format("HTTP/%u.%u", protocol_major, protocol_minor);
    _log.setPriorityLevel(Log::Logger::DEBUG);
}

// === Simple HTTP Request Handler ===
SimpleHttpRequestHandler::SimpleHttpRequestHandler(const String& path) :
HttpRequestHandler(0, 9),
_path(path)
{
    if (!FS::Exists(path))
        throw FS::Errors::FileNotFoundException(path);
}

bool SimpleHttpRequestHandler::http_get(HttpRequest& request)
{
    DateTime dt;
    _log.debug("get called: %s", request.uri().text());
    String output = String::Format("<h4>%s</h4><h1>SRL HTTP Server</h1>\n", dt.asString().text());
        
    request.sendResponse(output, "text/html");
    return true;
}

// === HTTP BASE CLASS ===

#define STARTING_THREADS 2

HttpServer::HttpServer(HttpRequestHandler& handler, uint32 max_cons) :
Thread(),
_socket_address(80),
_handler(handler),
_socket(),
_log(handler.getLogger()),
_pool_lock(),
_blocker(1),
_size(0),
_max_size(max_cons),
_available_pool(),
_busy_pool()
{
    for(uint32 i=0; i < STARTING_THREADS; ++i)
    {
        _available_pool.add(_createRequest());
    }
}

HttpServer::~HttpServer()
{
    // first lets cancel all the busy threads
    for(uint32 i=0; i < _busy_pool.size(); ++i)
    {
        _busy_pool[i]->cancel();
    }
    
    // next lets cancel all our paused threads
    for(uint32 i=0; i < _available_pool.size(); ++i)
    {
        _available_pool[i]->cancel(500);
        delete _available_pool[i];
    }
    
    // now lets go back and terminate any busy threads still alive
    for(uint32 i=0; i < _busy_pool.size(); ++i)
    {
        if (_busy_pool[i]->isAlive())
            _busy_pool[i]->terminate();
        delete _busy_pool[i];
    }
    
    
}

void HttpServer::start()
{
    Thread::start();
}

void HttpServer::start(const String& host, const uint32& port)
{
    InetSocketAddress addr(host, port);
    _socket_address = addr;
    start();
}

void HttpServer::stop()
{
    try
    {
        Thread::cancel(5000);
    }
    catch(SRL::Errors::Exception &e)
    {
        Thread::terminate();
    }
}

/** Starts the Server on the calling thread so this function should block forvever */
void HttpServer::serveForever()
{
    if (initial())
    {
        run();
    }
    final();
}

void HttpServer::serveForever(const String& host, const uint32& port)
{
    InetSocketAddress addr(host, port);
    _socket_address = addr;
    serveForever();
}

// ==== HTTP SERVER THREAD CALLBACKS ===
bool HttpServer::initial()
{
    _log.info("starting http server on: %s", _socket_address.asString().text());
    try
    {
        _socket.bind(_socket_address, true);
        _socket.listen();
    }
    catch(SRL::Errors::Exception &e)
    {
        _log.error(e);
        return false;
    }
    return true;
}

bool HttpServer::run()
{
    try
    {
		_log.info("listening for incoming requests");
        while (true)
        {
            HttpRequest* request = _getRequest();
            if (request == NULL)
            {
                // don't accept any new connections until we have a request object available
                _blocker.acquire();
                request = _getRequest();
                // this should never happen!!
                //if (request == NULL)
                //    return false;
            }
           request->accept(_socket.accept());
           request->unpause();
           doStateCheck(); 
        }
    }
    catch(SRL::Errors::Exception &e)
    {
        _log.error("unexpected error during accept");
        _log.error(e);
        return false;
    }
    return false;
}

void HttpServer::final()
{
    _log.info("http server shutting down");
    for( uint32 i = 0; i < _busy_pool.size(); ++i )
    {
        _busy_pool[i]->close();
    }
    _busy_pool.clear();
    _socket.close();
}


HttpRequest* HttpServer::_getRequest()
{
    _pool_lock.lock();
    HttpRequest* request = NULL;
    if (_available_pool.size() > 0)
    {
        request = _available_pool.pop();
    }    
    else if (_size < _max_size)
    {
        request = _createRequest();
    }
    else
    {
        _pool_lock.unlock();
        return NULL;
    }
    
    // now if we have reached capacity start blocking incoming requests
    if ((_available_pool.size() == 0) &  (_size == _max_size))
    {
        // pool is maxed out lock the blocker semaphore
        _blocker.tryAcquire();
    }
    
    _busy_pool.add(request);
    _pool_lock.unlock();
    return request;
}

void HttpServer::task_completed(Thread* thread)
{
    HttpRequest *request = (HttpRequest*)thread; 
    ScopedLock lock(_pool_lock);
    // everything in this scope is now synchronized
    thread->pause();
    //thread->setRunnable(NULL);
    
    _busy_pool.remove(request);
    _available_pool.add(request);
        
    if ((_available_pool.size() == 1) &  (_size == _max_size))
    {
        // pool is maxed out lock the waiter semaphore
        if (!_blocker.testAcquire())
            _blocker.release();
    }
}

HttpRequest* HttpServer::_createRequest()
{
    ++_size;
    HttpRequest* t = new HttpRequest((ThreadManager*)this, _handler);
    t->pause();
    t->start();
    return t;
}

