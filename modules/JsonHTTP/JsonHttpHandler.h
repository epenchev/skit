//
// JsonHttpHandler.h
// Copyright (C) 2014  Emil Penchev, Bulgaria


#ifndef JSON_REQUEST_HANDLER_
#define JSON_REQUEST_HANDLER_

#include "HttpServer.h"
#include "Socket.h"

#include <string>

class JSON_RequestHandler : public HttpServer::Listener,
                            public HttpSession::Listener
{
public:
	JSON_RequestHandler() {}
	virtual ~JSON_RequestHandler() {}

	static HttpServer::Listener* CreateListener() { return new JSON_RequestHandler; }

	// from HttpSession::Listener
	void OnHttpRequest( HttpSessionPtr session,
	                    Skit::HTTP::Request& request,
	                    const SysError& error );

	// from HttpServer::Listener
	bool OnHttpSession( HttpSessionPtr session, Skit::HTTP::Request& request );

private:
	// socket callback
	void OnSendHandler( HttpSessionPtr session,
	                    const SysError& error,
	                    size_t bytesWriten );

	void SendResponse(HttpSessionPtr session);
	string _data;
};

#endif //JSON_REQUEST_HANDLER_
