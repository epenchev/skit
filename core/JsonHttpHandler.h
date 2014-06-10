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
	void OnHttpRequest(HttpSessionPtr session, Skit::HTTP::Request& request);
private:
	void OnSendHandler(HttpSessionPtr session, const BoostErrCode& error, std::size_t sendBytes);
	std::string m_data;
};

#endif //JSON_REQUEST_HANDLER_
