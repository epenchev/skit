//
// HTTP_Server.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "http_server.h"
#include "Logger.h"
#include "Buffer.h"

#include <iostream>
#include <cstring>

/*
 * register HTTP_Server to the ServerController::ServerHandlerFactory
 */
ServerController::HandlerFactory::Registrator<HTTP_Server> reg("http");

HTTP_Server::HTTP_Server()
{
	std::set<CreateListenerFunc>::iterator it;
    for (it = HTTP_Server::GetRegistry().begin(); it != HTTP_Server::GetRegistry().end(); it++)
    {
    	HTTP_Server::Listener* listener = (*it)();
    	listeners_.insert(listener);
    }

    /* Build the response for the default session handler. */
    Skit::HTTP::Response response404;
    defaultResponse_ = response404.BuildResponse("404", "Not Found");
}

void HTTP_Server::acceptConnection(TCP_Socket* socket)
{
    HTTP_SessionPtr session(new HTTP_Session(socket));
    session->setListener(this);
    session->acceptRequest();
}

void HTTP_Server::onRequest(HTTP_SessionPtr session,
                            Skit::HTTP::Request& request, const SysError& error)
{
    bool handled = false;
    if (error)
    {
        /* Session will be disconnected and destroyed automatically
         * if no async I/O has been scheduled. */
        LOG(logERROR) << error.message();
        return;
    }
    else
    {
        set<HTTP_Server::Listener*>::iterator it;
        for (it = listeners_.begin(); it != listeners_.end(); it++)
        {
            handled = (*it)->onAcceptSession(session, request);
            if (handled)
            {
                /* We are done here, let the listener take care for this session. */
                HTTP_Session::Listener* sessListener = dynamic_cast<HTTP_Session::Listener*>(*it);
                if (sessListener)
                {
                    session->setListener(sessListener);
                    break;
                }
            }
        }
    }

    if (!handled)
    {
        static Buffer responseBuff((void*) defaultResponse_.data(),
                defaultResponse_.size());
        TcpSocket* sock = session->getSocket();
        sock->asyncWrite(BUFFER_SEQ(responseBuff),
                                   BIND_HANDLER(&HTTP_Server::OnSend));
    }
}

void HTTP_Server::OnSend(const SysError& error, size_t bytesWriten) {
    if (error)
        LOG(logERROR) << error.message();
    else
        LOG(logDEBUG) << "Session handled by the HTTP_Server default handler with response 404,"
                       "  bytes sent = " << bytesWriten;
}

HTTP_Session::HTTP_Session(TcpSocket* socket)
   : socket_(socket), buffer_(1024), listener_(NULL)
{
	reqdata_.clear();
}

HTTP_Session::~HTTP_Session() {
    //fSocketObj->Close();
}

/*
 * TODO build a better parser
 */
void HTTP_Session::onReceive(const TCP_Scoket::ErrCode& err, size_t bytesRead)
{
    if (err)
    {
        LOG(logERROR) << err.message();
        if (listener_)
        	listener_->onRequest(shared_from_this(), request_, err);

        return;
    }

    if (bytesRead && bytesRead <= buffer_.Size())
    {
        reqdata_ += buffer_.Get<const char*>();
        if (string::npos == reqdata_.find("\r\n\r\n"))
        {
            if (bytesRead < buffer_.Size())
            {
                LOG(logWARNING) << "Buffer not full no end marker, disconnecting";
                if (listener_)
                	listener_->onRequest(shared_from_this(), request_, error_);
            }
            else   /* collect the rest of the headers */
                acceptRequest();
        }
        else
        {
            request_.Init(reqdata_);
            if (listener_)
            	listener_->onRequest(shared_from_this(), request_, error_);

            reqdata_.clear();
        }
    }
    else
    {
        LOG(logWARNING) << "nothing to read must disconnect";
        if (listener_)
        	listener_->onRequest(shared_from_this(), request_, error_);
    }
}

void HTTP_Session::acceptRequest()
{
    memset(buffer_.Get<char*>(), 0, buffer_.Size());
    socket_->readSome(CreateBufferSequence(buffer_),
                                 BIND_SHARED_HANDLER(&HTTP_Session::onReceive));
}

void HTTP_Session::setListener(HTTP_Session::Listener* listener)
{
    if (listener)
    	listener_ = listener;
}

