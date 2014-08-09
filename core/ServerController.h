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

using namespace std;

class ServerHandler
{
public:
    virtual ~ServerHandler() {}

    // Factory creator func
    static ServerHandler* CreateItem() { return NULL; }

    // custom hook for incoming connection, implementation is in child classes
    virtual void AcceptConnection(TcpSocket* socket) = 0;

    // Get thread ID associated with this handler.
    ThreadID GetThreadId() const { return m_threadID; }

    // Set handler thread ID.
    void SetThread(ThreadID id) { m_threadID = id; }

protected:
    ThreadID m_threadID; // associated thread ID
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
    void Load(const string& filename);

    // From SocketAcceptorListener
    void OnAccept(TcpSocket* socket, ErrorCode& inError);

private:

    ServerController() {}
    ~ServerController() {}

    Skit::PropertyTree m_globalConfig;                          // xml global configuration
    boost::asio::io_service m_io_service;                       // parent (main) process io_service
    map<ThreadID, map<unsigned int, ServerHandler*> >m_servers; // server handlers
    list<SocketAcceptor*> m_acceptors;                          // socket acceptors listening for incoming connections
};


#endif // SERVER_CONTROLLER_H_
