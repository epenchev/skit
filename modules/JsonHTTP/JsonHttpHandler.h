//
// JsonHttpHandler.h
// Copyright (C) 2014  Emil Penchev, Bulgaria


#ifndef JSON_REQUEST_HANDLER_
#define JSON_REQUEST_HANDLER_

#include "http_server.h"
#include "socket.h"
#include "Buffer.h"
#include "Stream.h"

#include <string>

class JSON_RequestHandler : public HTTP_Server::Listener,
                            public HTTP_Session::Listener
{
public:
	JSON_RequestHandler() : outBuf_(NULL), player_(NULL), stream_(NULL) {}
	virtual ~JSON_RequestHandler() {}

	static HTTP_Server::Listener* CreateListener() { return new JSON_RequestHandler; }

	/** from HttpSession::Listener */
	void onRequest(HTTP_SessionPtr session,
	                   Skit::HTTP::Request& request,
	                   const SysError& error );

	/** from HttpServer::Listener */
	bool onAcceptSession(HTTP_SessionPtr session, Skit::HTTP::Request& request);

private:

	void handleRequest(HTTP_SessionPtr session, Skit::HTTP::Request& request);

	/** socket callback */
	void OnSendHandler(HTTP_SessionPtr session,
	                   const SysError& error,
	                   size_t bytesWriten);

	void SendResponse(HTTP_SessionPtr session);
	Buffer* outBuf_;
	string  responseHeaders_;
	Player* player_;
	Stream* stream_;
};

#endif //JSON_REQUEST_HANDLER_
