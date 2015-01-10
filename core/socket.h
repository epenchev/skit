//
// socket.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef SOCKET_H_
#define SOCKET_H_

#include "Buffer.h"
#include "ErrorCode.h"
#include "Task.h"

#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

/** bind handler functions with objects to be called on socket I/O completion */
#define BIND_HANDLER(func, ...) \
			boost::bind(func, this, ##__VA_ARGS__, \
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)

#define BIND_SHARED_HANDLER(func, ...) \
            boost::bind(func, shared_from_this(), ##__VA_ARGS__, \
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)

/**
 * Class for I/O operations over TCP network link, wrapper over boost::asio TCP socket
 */
class TCP_Socket : public boost::asio::ip::tcp::socket
{
public:
	TCP_Socket(boost::asio::io_service& io_service, ThreadID tid);
    virtual ~TCP_Socket();

    typedef boost::system::error_code ErrCode;

    /** Send data from buffer asynchronously, handler will be called on operation complete. */
    template <class ConstBufferSequence, class RunHandler>
    void asyncWrite(ConstBufferSequence buffer, RunHandler handler)
    {
    	boost::asio::async_write(*this, buffer, handler);
    }

    /**
     * Receive data into buffer asynchronously. Operation will complete when buffer is full.
     * When operation is complete handler will be called.
     */
    template <typename BufferSequence, typename RunHandler>
    void asyncRead(BufferSequence buffer, RunHandler handler)
    {
    	boost::asio::async_read(*this, buffer, handler);
    }

    /**
     * Receive data into buffer asynchronously. Will read only available data.
     *  When operation is complete handler will be called.
     */
    template <typename BufferSequence, typename RunHandler>
    void readSome(BufferSequence buffer, RunHandler handler)
    {
        async_read_some(buffer, handler);
    }

    /** Get the IP address to the connected endpoint, returns empty string on error. */
    std::string getRemoteAddress() const;

    /** Get the remote port to the connected endpoint, returns 0 on error. */
    unsigned short getRemotePort() const;
    
    /** Get the local listening port, returns 0 on error. */
    unsigned short getLocalPort() const;
    
    /** Return true if socket is open false otherwise. */
    bool isOpen() const;

    /** Shutdown communication link */
    void closedown();
    
    /** Get socket's io_service thread */
    ThreadID getThreadID() const { return tid_; }

    /** Listener to be notified when async connect operation is complete. */
    class ConnectListener
    {
    public:
    	virtual void onConnect(TCP_Socket& inSocket, ErrCode& err) = 0;
    	virtual ~ConnectListener() {}
    };

    /** Connect to remote host to netService as port number or service name. */
    void asyncConnect(std::string host, std::string service, ConnectListener* listener);

private:
    ThreadID tid_;   /** socket's io_service thread */

    /** boost socket handlers */
    void handleConnect(ConnectListener* listener, const ErrCode& err);
    void handleResolve(ConnectListener* listener, const ErrCode& err,
    				   boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

};

/** Accept incoming TCP connections */
class SocketAcceptor
{
public:

	/** Listener to be notified for every incoming TCP connection by SocketAcceptor instance. */
	class Listener
	{
	public:
	    /** Triggered when a new connection is accepted. */
	    virtual void onAccept(TCP_Socket* socket, TCP_Socket::ErrCode& err) = 0;
	    virtual ~Listener() {}
	};

    SocketAcceptor(unsigned short port, Listener* listener, boost::asio::io_service& io_service);
    SocketAcceptor(std::string address, unsigned short port, Listener* listener, boost::asio::io_service& io_service);

    ~SocketAcceptor();

    /** Close SocketAcceptor object and stop accepting incoming connections. */
    void stop();

    /** Return the port socket is listening on. */
    unsigned short getListeningPort() const;

    /** Listen for incoming connections. */
    void listen();

private:
    unsigned short                 listenPort_;     // listening port number
    std::string                    address_;        // listening IP address
    Listener*        			   listener_;       // event listener
    bool                           isListening_;    // is socket in listening state
    boost::asio::ip::tcp::acceptor impl_;           // boost acceptor

    /** Accepts incoming connections. */
    void accept();

    /** boost tcp::acceptor handler callback function */
    void handleAccept(TCP_Socket* socket, const TCP_Socket::ErrCode& error);
};

#endif /* SOCKET_H_ */

