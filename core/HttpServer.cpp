//
// HttpServer.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "HttpServer.h"
#include "Logger.h"
#include "Buffer.h"

#include <iostream>
#include <cstring>

/*
 * register HttpServer to the ServerController::ServerHandlerFactory
 */
ServerController::HandlerFactory::Registrator<HttpServer> reg("http");

HttpServer::HttpServer()
{
	for ( set<CreateListenerFunc>::iterator it  = HttpServer::GetRegistry().begin();
	      it != HttpServer::GetRegistry().end(); it++ )
	{
		HttpServer::Listener* listener = (*it)();
		fListeners.insert( listener );
	}

   /*
	* Build the response for the default session handler.
	*/
	Skit::HTTP::Response response404;
	fDefaultResponse = response404.BuildResponse("404", "Not Found");
}

void HttpServer::AcceptConnection(TcpSocket* socket)
{
	HttpSessionPtr session( new HttpSession( socket ) );
	session->SetListener(this);
	session->AcceptRequest();
}

void HttpServer::OnHttpRequest( HttpSessionPtr session,
                                Skit::HTTP::Request& request,
                                const SysError& error )
{
    bool handled = false;

    if (error)
    {
        /*
         * Session will be disconnected and destroyed automatically
         * if no async I/O has been scheduled.
         */
        LOG(logERROR) << error.message();
        return;
    }
    else
    {
        for ( set<HttpServer::Listener*>::iterator it = fListeners.begin();
              it != fListeners.end(); it++ )
        {
            handled = (*it)->OnHttpSession( session, request );
            if (handled)
            {
                /*
                 * We are done here, let the listener take care for this session.
                 */
                HttpSession::Listener* sessListener = dynamic_cast<HttpSession::Listener*>(*it);
                if (sessListener)
                {
                    session->SetListener(sessListener);
                    break;
                }
            }
        }
    }

    if (!handled)
    {
        static Buffer responseBuff( (void*)fDefaultResponse.data(), fDefaultResponse.size() );
        TcpSocket* sock = session->GetSocket();
        sock->Send( CreateBufferSequence(responseBuff),
                    BIND_HANDLER(&HttpServer::OnSend) );
    }
}

void HttpServer::OnSend( const SysError& error, size_t bytesWriten )
{
    if (error)
        LOG(logERROR) << error.message();
    else
        LOG(logDEBUG) << "Session handled by the HttpServer default handler with response 404,"
                         "  bytes sent = " << bytesWriten;
}

HttpSession::HttpSession( TcpSocket* socket )
 : fSocketObj(socket), fBufferObj(1024), fListener(NULL)
{
    fReqdata.clear();
}

HttpSession::~HttpSession()
{
    fSocketObj->Close();
}

/*
 * TODO build a better parser
 */
void HttpSession::OnReceive( const SysError& error, size_t bytesRead )
{
    if (error)
    {
    	LOG(logERROR) << error.message();
    	fError = error;
    	if (fListener)
    	    fListener->OnHttpRequest( shared_from_this(), fRequestObj, fError );

    	return;
    }

    if ( bytesRead &&
         bytesRead <= fBufferObj.Size() )
    {
        fReqdata += fBufferObj.Get<const char*>();
        if ( string::npos ==  fReqdata.find("\r\n\r\n") )
        {
        	if ( bytesRead < fBufferObj.Size() )
            {
        		LOG(logWARNING) << "Buffer not full no end marker, disconnecting";
        		if (fListener)
        		    fListener->OnHttpRequest( shared_from_this(), fRequestObj, fError );
            }
            else
                AcceptRequest();  // collect the rest of the headers
        }
        else
        {
            fRequestObj.Init(fReqdata);
            if (fListener)
                fListener->OnHttpRequest( shared_from_this(), fRequestObj, fError );
            fReqdata.clear();
        }
    }
    else
    {
    	LOG(logWARNING) << "nothing to read must disconnect";
    	if (fListener)
    	    fListener->OnHttpRequest( shared_from_this(), fRequestObj, fError );
    }
}

void HttpSession::AcceptRequest()
{
	memset( fBufferObj.Get<char*>(), 0, fBufferObj.Size() );
	fSocketObj->ReceiveSome( CreateBufferSequence(fBufferObj),
	                         BIND_SHARED_HANDLER(&HttpSession::OnReceive) );
}

void HttpSession::SetListener(HttpSession::Listener* listener)
{
    if (listener)
        fListener = listener;
}

