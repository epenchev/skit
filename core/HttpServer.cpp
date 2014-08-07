//
// HttpServer.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "HttpServer.h"
#include "Logger.h"
#include "Buffer.h"

#include <iostream>
#include <cstring>

// register HttpServer to the ServerController::ServerHandlerFactory
Skit::ServerController::HandlerFactory::Registrator<HttpServer> reg("http");

HttpServer::HttpServer()
{
	for ( set<CreateListenerFunc>::iterator it  = HttpServer::GetRegistry().begin();
	      it != HttpServer::GetRegistry().end(); it++ )
	{
		HttpServer::Listener* listener = (*it)();
		_listeners.insert( listener );
	}

   /*
	* Build the response for the default session handler.
	*/
	Skit::HTTP::Response response404;
	_defaultResponse = response404.BuildResponse("404", "Not Found");
}

void HttpServer::AcceptConnection(TcpSocketPtr socket)
{
	HttpSessionPtr session( new HttpSession( socket ) );
	session->SetListener( *this );
	session->AcceptRequest();
}

void HttpServer::OnHttpRequest( HttpSessionPtr session,
                                Skit::HTTP::Request& request,
                                const SysError& error )
{
    bool handled = false;

    if (error)
    {
        LOG(logERROR) << error.message();
        /*
         * Session will be disconnected and destroyed automatically
         * if no async I/O has been scheduled.
         */
        return;
    }
    else
    {
        for ( set<HttpServer::Listener*>::iterator it = _listeners.begin();
              it != _listeners.end(); it++ )
        {
            handled = (*it)->OnHttpSession( session, request );
            if ( handled )
            {
                /*
                * maybe handler has already attached as listener,
                * just to make sure we attached ourself ;)
                */
                HttpSession::Listener* sessListener = dynamic_cast<HttpSession::Listener*>(*it);
                if (sessListener)
                {
                    session->SetListener( *sessListener );
                    break;
                }
            }
        }
    }

    if (!handled)
    {
        static Buffer responseBuff( (void*)_defaultResponse.data(), _defaultResponse.size() );

        TcpSocketPtr sock = session->GetSocket();
        sock->Send( CreateBufferSequence(responseBuff),
                    BIND_HANDLER(&HttpServer::OnSend) );
    }
}

void HttpServer::OnSend( const SysError& error, size_t bytesWriten )
{
    if (error)
    {
        LOG(logERROR) << error.message();
    }
    else
    {
        LOG(logDEBUG) << "Session handled by the HttpServer default handler with response 404,"
                         "  bytes sent = " << bytesWriten;
    }
}

HttpSession::HttpSession( TcpSocketPtr socket )
 : _socketObj(socket), _bufferObj(1024), _listener(NULL)
{
    _reqdata.clear();
}

HttpSession::~HttpSession()
{
    _socketObj->Close();
}

/*
 * TODO build a better parser
 */
void HttpSession::OnReceive( const SysError& error, size_t bytesRead )
{
    if (error)
    {
    	LOG(logERROR) << error.message();
    	_error = error;

    	// TODO replace with smart pointer
    	if (_listener)
    	{
    	    _listener->OnHttpRequest( shared_from_this(), _requestObj, _error );
    	}

    	return;
    }

    if ( bytesRead &&
         bytesRead <= _bufferObj.Size() )
    {
        _reqdata += _bufferObj.Get<const char*>();
        if ( string::npos ==  _reqdata.find("\r\n\r\n") )
        {
        	if ( bytesRead < _bufferObj.Size() )
            {
        		LOG(logWARNING) << "Buffer not full no end marker, disconnecting";

        		// TODO replace with smart pointer
        		if (_listener)
        		{
        		    _listener->OnHttpRequest( shared_from_this(), _requestObj, _error );
        		}
            }
            else
            {
            	AcceptRequest();  // collect the rest of the headers
            }
        }
        else
        {
            _requestObj.Init(_reqdata);

            // TODO replace with smart pointer
            if (_listener)
            {
                _listener->OnHttpRequest( shared_from_this(), _requestObj, _error );
            }

            _reqdata.clear();
        }
    }
    else
    {
    	LOG(logWARNING) << "nothing to read must disconnect";

    	// TODO replace with smart pointer
    	if (_listener)
    	{
    	    _listener->OnHttpRequest( shared_from_this(), _requestObj, _error );
    	}
    }
}

void HttpSession::AcceptRequest()
{
	memset( _bufferObj.Get<char*>(), 0, _bufferObj.Size() );

	_socketObj->ReceiveSome( CreateBufferSequence(_bufferObj),
	                         BIND_SHARED_HANDLER(&HttpSession::OnReceive) );
}

void HttpSession::SetListener(HttpSession::Listener& listener)
{
    _listener = &listener;
}

