#include "srl/net/HttpHeaders.h"

using namespace SRL;
using namespace SRL::Net;

// to prevent a memory attack we set a maximum allowed headers
#define MAX_HEADERS 200

HttpHeaders::HttpHeaders(Log::Logger &logger) :
HttpHeaderMap(),
_log(logger)
{
    
}

HttpHeaders::~HttpHeaders()
{
    
}

void HttpHeaders::parse(TcpSocket& tcp_socket)
{
    String line(256);
    String name(64), value(64);
    clear();
    tcp_socket.readUntil(line, '\n', 10000);
    uint32 bytes_read = 0;
    
    // parse all of the headers until we reach an empty line
    while (!line.isEmpty() && (line != "\r\n") )
    {
        //printf("header line: '%s'\n", String::escape(line).text());
        // deal with multiline headers!! if a header starts with a space or tab it
        // belongs to the last header
        if (!name.isEmpty() && ((line[0] == ' ') || (line[0] == '\t')))
        {
            _log.debug("multiline header parsed");
            value += " ";
            value += line.between(':', '\r').trimEnd();
            (*this)[name] = value;
        }
        else if (line.find(':') > 0)
        {
            value = line.between(':', '\r').trim();
            name = line.section(':', 0).toLower();
            if (!hasKey(name))
            {
                _log.debug("header parsed: %s: %s", name.text(), value.text());
                add(name, value);
                // check if we have reached the maximum allowed headers
                if (size() >= MAX_HEADERS)
                {
                    throw SRL::Errors::PermissionDeniedException("the maximum allowed header count has been reached");
                }
            }
            else
            {
                _log.debug("duplicated header parsed: %s: %s", name.text(), value.text());
                // FIXME add support for duplicate headers
            }
        }
        // else invalid header
        line.clear();
        tcp_socket.readUntil(line, '\n', 10000);
    }    
}

