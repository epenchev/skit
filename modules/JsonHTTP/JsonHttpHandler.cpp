#include "HTTP.h"
#include "JsonHttpHandler.h"
#include "Logger.h"
#include "Buffer.h"

ListenerRegistrator<JSON_RequestHandler, HttpServer> regJsonListener;

void JSON_RequestHandler::OnSendHandler( HttpSessionPtr session,
                                         const SysError& error,
                                         size_t bytesWriten )
{
	LOG(logDEBUG) << "Send " << bytesWriten <<  " bytes";

}

bool JSON_RequestHandler::OnHttpSession( HttpSessionPtr session, Skit::HTTP::Request& request )
{
    LOG(logDEBUG) << request.GetURL();

    SendResponse(session);

    return true;
}

void JSON_RequestHandler::OnHttpRequest( HttpSessionPtr session,
                                         Skit::HTTP::Request& request,
                                         const SysError& error)
{
	LOG(logDEBUG) << request.GetURL();

	SendResponse(session);
}

void JSON_RequestHandler::SendResponse( HttpSessionPtr session )
{
    Skit::HTTP::Response response;
    _data = response.BuildResponse("200", "OK");
    _data += "Hello World";

    Buffer buf((void*)_data.c_str(), _data.size());

    TcpSocketPtr socket = session->GetSocket();

    socket->Send( CreateBufferSequence(buf),
                  BIND_HANDLER(&JSON_RequestHandler::OnSendHandler,
                  session) );
}
