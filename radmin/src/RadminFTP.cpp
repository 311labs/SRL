#include "srl/radmin/RadminFTP.h"

using namespace SRL;
using namespace SRL::Radmin;
using namespace SRL::Net;

RadminFTP::RadminFTP(int port, const String& path, bool get_file):
_is_getting(get_file), _is_done(false), _server(NULL), _connection(NULL),
_file(NULL), _path(path), _username()
{
    _server = new SRL::Net::ServerSocket(port);
    _server->startListener(this);

}
/** default contstructor */
RadminFTP::RadminFTP(int port, const String& path, const String& username, bool get_file) : 
_is_getting(get_file), _is_done(false), _server(NULL), _connection(NULL), 
_file(NULL), _path(path), _username(username)
{
    _server = new ServerSocket(port);
    _server->startListener(this);    
}            
/** default destructor */
RadminFTP::~RadminFTP()
{
    
    if (_file != NULL)
    {
        // close will delete the file
        _file->close();
        _file = NULL;
        if ((!_is_getting)&&(_username.length() > 1))
        {
            System::UserInfo user_info;
            System::GetUserInfo(_username, user_info);
            FS::ChangeOwner(_path, user_info.uid, user_info.gid);
        }
    }
    
    _server->stopListener();
    if (_connection != NULL)
    {
        delete _connection;
    }    
    delete _server;
}

void RadminFTP::connection_new(Socket::Descriptor &desc)
{
    _connection = new SRL::Net::TcpSocket(desc);

    if (_is_getting)
    {
        _file = new SRL::FS::BinaryFile(_path, SRL::FS::File::BINARY_READ, true);

        int bytes_to_send = 0;
        try
        {
            while (!_file->atEnd())
            {
                bytes_to_send = _file->read(_buffer, 1, 255);
                if (bytes_to_send > 0)
                {
                    _connection->send(_buffer, bytes_to_send);
                }
            }
        }
        catch (Errors::Exception &e)
        {
            // TODO get logger support here
            //_logger->error(e.message());
        }
        _file->close();
        _file = NULL;
		_connection->close();
		_server->close();
		_server->stopListener();
    }
    else
    {
        _file = new SRL::FS::BinaryFile(_path, SRL::FS::File::BINARY_WRITE, true);
		// now lets force the calling thread to wait for receive
		while (_connection->isConnected())
		{
			_server->waitForRecv(15000);
		}
        _file->close();
        _file = NULL;
		_connection->close();
		_server->close();
		_server->stopListener();
    }
}

void RadminFTP::connection_recvReady(Socket::Descriptor &desc)
{
    try
    {
        int bytes_read = _connection->recv(_buffer, 255);
        if (bytes_read > 0)
        {
            _file->write(_buffer, bytes_read);
        }
    }
    catch (Errors::Exception &e)
    {
        // TODO get logger support here
        //_logger->error(e.message());
    }
}

void RadminFTP::connection_closed(Socket::Descriptor &desc)
{
    _is_done = true;
}
