#include "HTTP.h"
#include "JsonHttpHandler.h"
#include "Logger.h"
#include "Buffer.h"

ListenerRegistrator<JSON_RequestHandler, HttpServer> regJsonListener;

void JSON_RequestHandler::OnSendHandler(HttpSessionPtr session, const boost::system::error_code& error, std::size_t sendBytes)
{
	LOG(logDEBUG) << "Send " << sendBytes <<  " bytes";
	session->Disconnect();

}

void JSON_RequestHandler::OnHttpRequest(HttpSessionPtr session, Skit::HTTP::Request& request)
{
	LOG(logDEBUG) << request.GetURL();
	Skit::HTTP::Response response;
	m_data = response.BuildResponse("200", "OK");
	m_data += "Hello World";

	Buffer buf((void*)m_data.c_str(), m_data.size());


	TcpSocketPtr socket = session->GetSocket();
	socket->Send(CreateBufferSequence(buf), BIND_HANDLER(&JSON_RequestHandler::OnSendHandler, session));
}

