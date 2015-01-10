//
// Socket.cpp
// Copyright (C) 2013  Emil Penchev, Bulgaria
 
#include "socket.h"
#include "Logger.h"

using boost::asio::ip::tcp;

TCP_Socket::TCP_Socket(boost::asio::io_service& io_service, ThreadID tid)
 : boost::asio::ip::tcp::socket(io_service), tid_(tid)
{}

TCP_Socket::~TCP_Socket()
{
    closedown();
}

std::string TCP_Socket::getRemoteAddress() const
{
    std::string peer = "";
    if (is_open())
    {
    	ErrCode err;
        boost::asio::ip::tcp::endpoint endpoint = remote_endpoint(err);
        if (!err)
        {
            peer = endpoint.address().to_string(err);
            if (err)
            {
                LOG(logERROR) << err.message();
                peer = "";
            }
        }
        else
        	LOG(logERROR) << err.message();
    }

    return peer;
}

unsigned short TCP_Socket::getRemotePort() const
{
    if (is_open())
    {
    	ErrCode err;
        boost::asio::ip::tcp::endpoint endpoint = remote_endpoint(err);
        if (!err)
            return endpoint.port();

        LOG(logERROR) << err.message();
    }

    return 0;
}

unsigned short TCP_Socket::getLocalPort() const
{
    if (is_open())
    {
    	ErrCode err;
        boost::asio::ip::tcp::endpoint endpoint = local_endpoint(err);
        if (!err)
            return endpoint.port();

        LOG(logERROR) << err.message();
    }

    return 0;
}

void TCP_Socket::asyncConnect(std::string host, std::string service, ConnectListener* listener)
{
	ErrCode err;
    boost::asio::ip::address address = boost::asio::ip::address::from_string(host, err);
    if (err)
    {
        /* host is not IP address, resolve it */
        tcp::resolver resolver(get_io_service());
        tcp::resolver::query query(host, service);
        resolver.async_resolve(query,
            boost::bind(&TCP_Socket::handleResolve, this, listener,
                boost::asio::placeholders::error, boost::asio::placeholders::iterator));
    }
    else
    {
        if (!service.empty())
        {
            boost::asio::ip::tcp::endpoint endpoint(address, atoi(service.c_str()));
            async_connect(endpoint, boost::bind(&TCP_Socket::handleConnect, this,
            		                            listener, boost::asio::placeholders::error));
        }
    }
}

void TCP_Socket::closedown()
{
    if (is_open())
    {
        /* no error handling here nobody cares, just skip exceptions */
        boost::system::error_code error;
        shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
        /* prevent exception throw from boost */
        close(error);
    }
}

void TCP_Socket::handleResolve(ConnectListener* listener,
                              const ErrCode& err,
		                      tcp::resolver::iterator endpoint_iterator)
{
    if (err)
    {
        if (boost::asio::error::operation_aborted == err)
            return; /* operation aborted, don't signal */

        /* signal only in case of error */
        listener->onConnect(*this, err);
    }
    else
        boost::asio::async_connect(*this, endpoint_iterator,
            boost::bind(&TCP_Socket::handleConnect, this, listener, boost::asio::placeholders::error));
}

void TCP_Socket::handleConnect(ConnectListener* listener, const ErrCode& err)
{
    ErrorCode outErr;
    if (err)
    {
        if (boost::asio::error::operation_aborted == err)
            return; /* operation aborted, don't signal */
    }
    listener->onConnect(*this, err);
}

bool TCP_Socket::isOpen() const
{
    return is_open();
}

SocketAcceptor::SocketAcceptor(unsigned short port, Listener* listener, boost::asio::io_service& io_service)
 : listenPort_(port), listener_(listener), isListening_(false), impl_(io_service)
{}

SocketAcceptor::SocketAcceptor(std::string localAdress, unsigned short port,
                   Listener* listener, boost::asio::io_service& io_service)
 : listenPort_(port), address_(localAdress), listener_(listener),
    isListening_(false), impl_(io_service)
{}

SocketAcceptor::~SocketAcceptor()
{
    stop();
}

void SocketAcceptor::listen()
{
    try
    {
        if (isListening_)
        	return; // already in state listen.

        impl_.open(boost::asio::ip::tcp::v4());
        if (!address_.empty())
        {
            boost::asio::ip::address bindAdress;
            bindAdress.from_string(address_);
            impl_.bind(tcp::endpoint(bindAdress, listenPort_));
        }
        else
        	impl_.bind(tcp::endpoint(tcp::v4(), listenPort_));

        impl_.set_option(tcp::acceptor::reuse_address(true));
        impl_.listen();
    }
    catch(boost::system::system_error& ex)
    {
        ErrorCode outError;
        outError.SetMessage(ex.code().message());
        throw SystemException(outError);
    };
    isListening_ = true;
    this->accept();
}

void SocketAcceptor::stop()
{
    if (isListening_)
    {
        boost::system::error_code error; // no error handling here just disable exceptions
        impl_.close(error);
        isListening_ = false;
    }
}

void SocketAcceptor::accept()
{
    TaskScheduler& scheduler = TaskScheduler::Instance();
    try
    {
        ThreadID tid = scheduler.GetNextThread();
        TCP_Socket* sock = new TCP_Socket(scheduler.GetThreadIOService(tid), tid);
        impl_.async_accept(*sock, boost::bind(&SocketAcceptor::handleAccept,
        		                                      this, sock, boost::asio::placeholders::error));
    }
    catch (TaskSchedulerException& ex)
    {
        LOG(logERROR) << "TaskScheduler exception caught : " << ex.what();
    }
    catch(std::bad_alloc &ex)
    {
        LOG(logERROR) << "Error allocating memory : " << ex.what();
    }
}

unsigned short SocketAcceptor::getListeningPort() const
{
    unsigned short socketPort = 0;
    if (isListening_)
    {
        TCP_Socket::ErrCode err;
        tcp::endpoint epoint = impl_.local_endpoint(err);
        if (!err)
        	socketPort = epoint.port();
    }
    return socketPort;
}

void SocketAcceptor::handleAccept(TCP_Socket* socket, const TCP_Socket::ErrCode& err)
{
    if (err)
    {
        delete socket;
        if (boost::asio::error::operation_aborted == err)
        {
            LOG(logDEBUG) << "Aborting";
            /* operation aborted, don't signal */
            return;
        }
    }

    /* warning, if error is set socket is invalid object */
    if (listener_)
    	listener_->onAccept(socket, err);

    /* Continue accepting new connections automatically. */
    this->accept();
}

