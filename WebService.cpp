/*
 * WebService.cpp
 *
 * Copyright (C) 2013  Emil Penchev, Bulgaria
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Created on: Mar 28, 2013
 *      Author: emo
 */

#include "WebService.h"
#include "Log.h"
#include "HTTPParser.h"

namespace blitz {

WebSession::WebSession(boost::asio::io_service& io_service) : TCPConnection(io_service)
{}

void WebSession::start(void)
{
    TCPConnection::start();

    if (isConnected() && socket().is_open())
    {
        BLITZ_LOG_INFO("Web service connected from: %s, from port:%d",
                                          getRemoteIP().to_string().c_str(), getRemotePort());

        boost::asio::async_read_until(socket(), m_response, "\r\n\r\n",
                                      boost::bind(&WebSession::handleReadCommand, this,
                                         boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
    }
}

void WebSession::close(void)
{
    TCPConnection::close();
    m_state = SERVICE_CLOSED;
    notify();
}

void WebSession::sendData(const void* data, std::size_t size)
{
    boost::asio::async_write(socket(), boost::asio::buffer(data, size),
                             boost::bind(&WebSession::handleSendResponse, this,
                               boost::asio::placeholders::error));
}

void WebSession::handleReadCommand(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error)
    {
        m_state = SERVICE_REQ_RECEIVED;
        notify();
        m_response.consume(bytes_transferred);
    }
    else // error
    {
        BLITZ_LOG_ERROR("WebSession::handleReadCommand %s", error.message().c_str());
        close();
    }
}

void WebSession::handleSendResponse(const boost::system::error_code& error)
{
    if (error)
    {
        BLITZ_LOG_ERROR("error: %s", error.message().c_str());
    }
    close();
}

WebService::WebService(boost::asio::io_service& io_service, const unsigned int tcp_port)
 : TCPServer(io_service, tcp_port), m_client(io_service)
{
    m_client.attach(this);
}

TCPConnection* WebService::createTCPConnection(boost::asio::io_service& io_service)
{
    /* remove unused and closed sessions */
    BLITZ_LOG_INFO("sessions active:%d , to be deleted: %d",m_conn_pool.size(), m_orphane_sessions.size());

    clearSessions();

    WebSession* conn = new WebSession(io_service);
    conn->attach(this);

    return conn;
}

void WebService::update(Subject* changed_subject)
{
    if (changed_subject)
    {
        blitz::http::HTTPClient* client = NULL;
        try
        {
            client = dynamic_cast<blitz::http::HTTPClient*>(changed_subject);
        }
        catch(std::exception& e)
        {
            BLITZ_LOG_ERROR("bad cast exception: %s", e.what());
            return;
        }

        if (0 == client)
        {
            BLITZ_LOG_INFO("update from WebSession ");
            WebSession* session = NULL;
            try
            {
                session = dynamic_cast<WebSession*>(changed_subject);
            }
            catch(std::exception& e)
            {
                BLITZ_LOG_ERROR("bad cast exception from WebSession: %s", e.what());
                return;
            }

            WebSessionState state = session->getState();

            if (SERVICE_REQ_RECEIVED == state)
            {
                boost::asio::streambuf& data = session->getData();
                blitz::HTTPParser parser;

                if (parser.isValid(data, HTTPReq))
                {
                    HTTPRequest& req = parser.getRequest();
                    std::string req_type = req.getRequestType();

                    if ("GET" == req_type)
                    {
                        std::vector<http_param> vparams;
                        std::string target = req.getParams(vparams);

                        blitz::Command cmd(target);

                        for (unsigned i = 0; i < vparams.size(); i++)
                            cmd.insertParameter(vparams[i]);

                        bool executed = false;

                        for (std::list<blitz::Controler*>::iterator it = m_controlers.begin(); it != m_controlers.end(); ++it)
                        {
                            std::string cmd_result;
                            blitz::Controler* controler = *it;

                            if (controler->execCommand(cmd, cmd_result))
                            {
                                executed = true;
                                HTTPResponse http_response;
                                http_response.data(200, cmd_result.size());
                                std::string buf = http_response.getHeaders() + cmd_result;
                                session->sendData(buf.c_str(), buf.size());
                            }
                        }

                        if (!executed)
                        {
                            // Send HTTP error code Not Found
                            HTTPResponse http_response;
                            http_response.data(404);
                            session->sendData(http_response.getHeaders().c_str(), http_response.getHeaders().size());
                        }
                    }
                }
            }
            else if (SERVICE_CLOSED == state)
            {
                BLITZ_LOG_WARNING("session closed");

                // remove connection from pool
                m_conn_pool.erase(session);

                // set to be destroyed later
                m_orphane_sessions.push_back(session);
            }
        }
        else
        {
            BLITZ_LOG_INFO("update from HTTPClient");

            blitz::http::HTTPClientState state = m_client.getState();
            if (blitz::http::STATE_DATARECV == state)
            {
                /* Unused for now
                boost::asio::streambuf& data = m_client.getContent();
                */
                BLITZ_LOG_INFO("STATE_DATARECV from HTTPClient");
            }
        }
    }
}

void WebService::clearSessions(void)
{
    if (!m_orphane_sessions.empty())
    {
        for (std::vector<WebSession*>::iterator it = m_orphane_sessions.begin();
                                                    it != m_orphane_sessions.end(); ++it)
        {
            WebSession* session = *it;
            delete session;
        }
        m_orphane_sessions.clear();
    }
}

void WebService::registerControler(Controler* contrl)
{
    if (contrl)
    {
        m_controlers.push_back(contrl);
        contrl->setChannel(this);
    }
}

void WebService::unregisterControler(Controler* contrl)
{
    if (contrl)
    {
        m_controlers.remove(contrl);
        contrl->closeChannel(this);
    }
}

void WebService::triggerEvent(ControlerEvent& ev)
{

}

} // blitz
