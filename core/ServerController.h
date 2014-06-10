//
// Server.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef SERVER_CONTROLLER_H_
#define SERVER_CONTROLLER_H_

#include "Task.h"
#include "Socket.h"
#include "PropertyTree.h"
#include "RegFactory.h"

#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<TcpSocket> TcpSocketPtr;

namespace Skit {

class ServerHandler
{
public:
    virtual ~ServerHandler() {}

    // Factory creator func
    static ServerHandler* CreateItem() { return NULL; }

    // custom hook for incoming connection, implementation is in child classes
    virtual void AcceptConnection(TcpSocketPtr socket) = 0;

    // Get thread ID associated with this handler.
    Skit::ThreadID GetThreadId() const { return m_threadID; }

    // Set handler thread ID.
    void SetThread(Skit::ThreadID id) { m_threadID = id; }

protected:
    Skit::ThreadID m_threadID; // associated thread ID
};

// main server/stream controller
class ServerController : public SocketAcceptor::Listener
{
public:
	// Factory for registering all the server handler classes
	typedef RegFactory<ServerHandler> HandlerFactory;

    static ServerController& Instance()
    {
        static ServerController s_mainServer;
        return s_mainServer;
    }

    // read server configuration and start.
    void Load(const std::string& filename);

    // From SocketAcceptorListener
    void OnAccept(TcpSocket* socket, ErrorCode& inError);

private:

    ServerController() {}
    ~ServerController() {}

    Skit::PropertyTree m_globalConfig;                        // xml global configuration
    boost::asio::io_service m_io_service;                     // parent (main) process io_service
    std::map<Skit::ThreadID, std::map<unsigned int, Skit::ServerHandler*> >m_servers; // server handlers
    std::list<SocketAcceptor*> m_acceptors;                   // socket acceptors listening for incoming connections
};

} // Skit

#endif // SERVER_CONTROLLER_H_
