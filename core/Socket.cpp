//
// Socket.cpp
// Copyright (C) 2013  Emil Penchev, Bulgaria
 
#include "Socket.h"
#include "Logger.h"

using boost::asio::ip::tcp;

TcpSocket::TcpSocket(boost::asio::io_service& io_service, Skit::ThreadID threadID)
 : boost::asio::ip::tcp::socket(io_service), m_threadID(threadID)
{}

TcpSocket::~TcpSocket()
{
    Close();
}

std::string TcpSocket::GetRemoteIP() const
{
    std::string peer = "";

    if (is_open())
    {
    	BoostErrCode error;
        boost::asio::ip::tcp::endpoint endpoint = remote_endpoint(error);
        if (error)
        {
            LOG(logERROR) << error.message();
        }
        else
        {
            peer = endpoint.address().to_string(error);
            if (error)
            {
                LOG(logERROR) << error.message();
                peer = "";
            }
        }
    }

    return peer;
}

unsigned short TcpSocket::GetRemotePort() const
{
    if (is_open())
    {
    	BoostErrCode error;
        boost::asio::ip::tcp::endpoint endpoint = remote_endpoint(error);
        if (!error)
        {
            return endpoint.port();
        }
        else
        {
            LOG(logERROR) << error.message();
            return 0;
        }
    }

    return 0;
}

unsigned short TcpSocket::GetLocalPort() const
{
    if (is_open())
    {
    	BoostErrCode error;
        boost::asio::ip::tcp::endpoint endpoint = local_endpoint(error);
        if (!error)
        {
            return endpoint.port();
        }
        else
        {
            LOG(logERROR) << error.message();
            return 0;
        }
    }

    return 0;
}

void TcpSocket::Connect(std::string host, std::string netService, ConnectListener* listener)
{
	BoostErrCode error;
    boost::asio::ip::address hostIP = boost::asio::ip::address::from_string(host, error);

    if (error)
    {
        // host is not IP address, resolve it
        tcp::resolver resolver(get_io_service());
        tcp::resolver::query query(host, netService);

        resolver.async_resolve(query,
            boost::bind(&TcpSocket::HandleResolve, this, listener,
                boost::asio::placeholders::error, boost::asio::placeholders::iterator));
    }
    else
    {
        if (!netService.empty())
        {
            boost::asio::ip::tcp::endpoint endpoint(hostIP, atoi(netService.c_str()));
            async_connect(endpoint, boost::bind(&TcpSocket::HandleConnect, this, listener, boost::asio::placeholders::error));
        }
    }
}

void TcpSocket::Close()
{
    if (is_open())
    {
        // no error handling here nobody cares, just skip exceptions
        boost::system::error_code error;
        shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
        close(error);
    }
}

void TcpSocket::HandleResolve(ConnectListener* listener,
                              const BoostErrCode& error,
		                      tcp::resolver::iterator endpoint_iterator)
{
	ErrorCode outErr;
    if (error)
    {
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted, don't signal
        }
        outErr = true;
        
        // signal only in case of error
        listener->OnConnect(*this, outErr);
    }
    else
    {
        boost::asio::async_connect(*this, endpoint_iterator,
            boost::bind(&TcpSocket::HandleConnect, this, listener, boost::asio::placeholders::error));
    }
}

void TcpSocket::HandleConnect(ConnectListener* listener, const BoostErrCode& error)
{
    ErrorCode outErr;
    if (error)
    {
        if (boost::asio::error::operation_aborted == error)
        {
            return; // operation aborted, don't signal
        }
        outErr = true;
    }
    listener->OnConnect(*this, outErr);
}

bool TcpSocket::IsOpen() const
{
    return is_open();
}

// 

SocketAcceptor::SocketAcceptor(unsigned short port, Listener* listener, boost::asio::io_service& io_service)
 : m_listenPort(port), m_listener(listener), m_isListening(false), m_acceptorImpl(io_service)
{}

SocketAcceptor::SocketAcceptor(std::string localAdress, unsigned short port,
                   Listener* listener, boost::asio::io_service& io_service)
 : m_listenPort(port), m_listenAddress(localAdress), m_listener(listener),
    m_isListening(false), m_acceptorImpl(io_service)
{}

SocketAcceptor::~SocketAcceptor()
{
    Stop();
}

void SocketAcceptor::Listen()
{
    try
    {
        if (m_isListening)
        {
            return; // already in state listen.
        }
        m_acceptorImpl.open(boost::asio::ip::tcp::v4());
        if (!m_listenAddress.empty())
        {
            boost::asio::ip::address bindAdress;
            bindAdress.from_string(m_listenAddress);
            m_acceptorImpl.bind(tcp::endpoint(bindAdress, m_listenPort));
        }
        else
        {
            m_acceptorImpl.bind(tcp::endpoint(tcp::v4(), m_listenPort));
        }
        m_acceptorImpl.set_option(tcp::acceptor::reuse_address(true));
        m_acceptorImpl.listen();
    }
    catch(boost::system::system_error& ex)
    {
        ErrorCode outError;
        outError.SetMessage(ex.code().message());
        throw SystemException(outError);

    };
    m_isListening = true;
    Accept();
}

void SocketAcceptor::Stop()
{
    if (m_isListening)
    {
        boost::system::error_code error; // no error handling here just disable exceptions
        m_acceptorImpl.close(error);
        m_isListening = false;
    }
}

void SocketAcceptor::Accept()
{
    Skit::TaskScheduler& scheduler = Skit::TaskScheduler::Instance();
    Skit::ThreadID id = scheduler.GetNextThread();
    try
    {
        TcpSocket* sock = new TcpSocket(scheduler.GetThreadIOService(id), id);
        m_acceptorImpl.async_accept(*sock, boost::bind(&SocketAcceptor::HandleAccept, this, sock, boost::asio::placeholders::error));
    }
    catch(Skit::TaskSchedulerException& ex)
    {
        LOG(logERROR) << "exception caught: " << ex.what();
    }
}

unsigned short SocketAcceptor::GetListeningPort() const
{
    unsigned short socketPort = 0;
    if (m_isListening)
    {
        boost::system::error_code err;
        tcp::endpoint epoint = m_acceptorImpl.local_endpoint(err);
        if (!err)
        {
            socketPort = epoint.port();
        }
    }
    return socketPort;
}

void SocketAcceptor::HandleAccept(TcpSocket* socket, const boost::system::error_code& error)
{
    m_error = false;
    if (error)
    {
    	m_error.SetMessage(error.message());
        delete socket;
        if (boost::asio::error::operation_aborted == error)
        {
            LOG(logDEBUG) << "Aborting";
            return; // operation aborted, don't signal
        }
    }

    // warning, error is set socket is invalid object
    if (m_listener)
    {
        m_listener->OnAccept(socket, m_error);
    }

    // continue accepting new connections.
    Accept();
}

