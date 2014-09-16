//
// ServerController.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef SERVER_CONTROLLER_H_
#define SERVER_CONTROLLER_H_

#include "Stream.h"
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
    ThreadID GetThreadId() const { return _threadID; }

    // Set handler thread ID.
    void SetThread(ThreadID id) { _threadID = id; }

protected:
    ThreadID _threadID; // associated thread ID
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

    Stream* GetStream(string name);

private:

    ServerController();
    ~ServerController() {}

    void LoadStreams();

    PropertyTree                                       fGlobalConfigTree;    // xml global configuration
    boost::asio::io_service                            fIOService;           // main process io_service, used to accept incoming connections
    list<SocketAcceptor*>                              fAcceptors;           // socket acceptors listening for incoming connections
    map<string, Stream*>                               fStreams;             // all media streams
    map<ThreadID, map<unsigned int, ServerHandler*> >  fServerHandlers;      // server handlers
};


#endif // SERVER_CONTROLLER_H_
