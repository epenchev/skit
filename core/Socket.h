//
// Socket.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef SOCKET_H_
#define SOCKET_H_

#include "Buffer.h"
#include "ErrorCode.h"
#include "Task.h"

#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

// bind handler functions with objects to be called on socket I/O completion
#define BIND_HANDLER(func, ...) \
			boost::bind(func, this, ##__VA_ARGS__, \
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)

#define BIND_SHARED_HANDLER(func, ...) \
            boost::bind(func, shared_from_this(), ##__VA_ARGS__, \
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)


typedef boost::system::error_code SysError;

// Class for I/O operations over TCP network link
class TcpSocket : public boost::asio::ip::tcp::socket
{
public:
    TcpSocket(boost::asio::io_service& io_service, Skit::ThreadID threadID);
    virtual ~TcpSocket();

    // Send data from buffer asynchronously, handler will be called on operation complete.
    template <class ConstBufferSequence, class RunHandler>
    void Send(ConstBufferSequence buffer, RunHandler handler)
    {
    	boost::asio::async_write(*this, buffer, handler);
    }

    // Receive data into buffer asynchronously. Operation will complete when buffer is full.
    // When operation is complete handler will be called.
    template <typename BufferSequence, typename RunHandler>
    void Receive(BufferSequence buffer, RunHandler handler)
    {
    	boost::asio::async_read(*this, buffer, handler);
    }

    // Receive data into buffer asynchronously. Will read only available data.
    // When operation is complete handler will be called.
    template <typename BufferSequence, typename RunHandler>
    void ReceiveSome(BufferSequence buffer, RunHandler handler)
    {
    	async_read_some(buffer, handler);
    }

    // Get the IP address to the connected endpoint, returns empty string on error.
    std::string GetRemoteIP() const;

    // Get the remote port to the connected endpoint, returns 0 on error.
    unsigned short GetRemotePort() const;
    
    // Get the local listening port, returns 0 on error.
    unsigned short GetLocalPort() const;
    
    // Return true if socket is open false otherwise.
    bool IsOpen() const;

    // Disconnect from remote host.
    void Close();
    
    // Get socket's io_service thread
    Skit::ThreadID GetThreadID() const { return m_threadID; }

    class ConnectListener
    {
    public:
    	virtual void OnConnect(TcpSocket& inSocket, ErrorCode& inError) = 0;
    	virtual ~ConnectListener() {}
    };

    // Connect to remote host to netService as port number or service name.
    void Connect(std::string host, std::string netService, ConnectListener* listener);

private:
    Skit::ThreadID m_threadID;                 // socket's io_service thread

    // boost socket IO handlers for connect
    void HandleConnect(ConnectListener* listener, const SysError& err);
    void HandleResolve(ConnectListener* listener, const SysError& err,
    						boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

};

// Accept incoming TCP connections
class SocketAcceptor
{
public:

	// Listener to be notified for every incoming TCP connection by SocketAcceptor instance.
	class Listener
	{
	public:
	    // Triggered when a new connection is accepted.
	    virtual void OnAccept(TcpSocket* socket, ErrorCode& inError) = 0;

	    virtual ~Listener() {}
	};

    SocketAcceptor(unsigned short port, Listener* listener, boost::asio::io_service& io_service);
    SocketAcceptor(std::string localAddress, unsigned short port, Listener* listener, boost::asio::io_service& io_service);

    ~SocketAcceptor();

    // Close SocketAcceptor object and stop accepting incoming connections.
    void Stop();

    // Return the port socket is listening on.
    unsigned short GetListeningPort() const;

    // Listen for incoming connections.
    void Listen();

private:
    unsigned short                 m_listenPort;     // listening port number
    std::string                    m_listenAddress;  // listening IP address
    Listener*        			   m_listener;       // event listener
    bool                           m_isListening;    // is socket in listening state
    ErrorCode                      m_error;          // error code of accept operation
    boost::asio::ip::tcp::acceptor m_acceptorImpl;   // boost acceptor

    // Accepts incoming connections.
    void Accept();

    // boost tcp::acceptor handler callback function
    void HandleAccept(TcpSocket* socket, const SysError& error);
};

#endif /* SOCKET_H_ */

