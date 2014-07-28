//
// JsonHttpHandler.h
// Copyright (C) 2014  Emil Penchev, Bulgaria


#ifndef JSON_REQUEST_HANDLER_
#define JSON_REQUEST_HANDLER_

#include "HttpServer.h"
#include "Socket.h"

#include <string>

class JSON_RequestHandler : public HttpServer::ReqListener
{
public:
	JSON_RequestHandler() {}
	virtual ~JSON_RequestHandler() {}

	static HttpServer::ReqListener* CreateListener() { return new JSON_RequestHandler; }
	void OnHttpRequest(HttpSessionPtr session, Skit::HTTP::Request& request);

private:
	void OnSendHandler(HttpSessionPtr session, const SysError& error, std::size_t sendBytes);
	std::string m_data;
};

#endif //JSON_REQUEST_HANDLER_
