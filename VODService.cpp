/*
 * VODService.cpp
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
 *  Created on: Feb 28, 2013
 *      Author: emo
 */

#include <boost/lexical_cast.hpp>
#include "VODService.h"
#include "HTTPParser.h"
#include "Log.h"

#include <map>
#include <stdio.h>
#include <time.h>

namespace blitz {

VODService::VODService(boost::asio::io_service& io_service, const unsigned int tcp_port,
                       std::string ip_address, std::string filepath)
: TCPServer(io_service, tcp_port), m_ip_address(ip_address), m_basepath(filepath), m_port(tcp_port)
{
    BLITZ_LOG_INFO("Create VOD service");
}

TCPConnection* VODService::createTCPConnection(boost::asio::io_service& io_service)
{
    /* remove unused and closed sessions */
    BLITZ_LOG_INFO("sessions active:%d , to be deleted: %d",m_conn_pool.size(), m_orphane_sessions.size());
    clearSessions();

    VODSession* session = new VODSession(io_service);

    // attach observer
    session->attach(this);

    return session;
}

void VODService::update(Subject* changed_subject)
{
    if (changed_subject)
    {
        VODSession* session = NULL;
        try
        {
            session = dynamic_cast<VODSession*>(changed_subject);
        }
        catch(std::exception& e)
        {
            BLITZ_LOG_ERROR("bad cast exception: %s", e.what());
            return;
        }

        VODSessionState sess_state = session->getState();
        if (VOD_STATE_REQ_RECEIVED == sess_state)
        {
            BLITZ_LOG_INFO("resource is: %s", session->getResouceId().c_str());
            std::map<std::string ,std::string>::iterator it;

            blitz::record_id rec_id;
            MediaSessionDB::Record rec;
            std::stringstream hex_converter;

            hex_converter << std::hex << session->getResouceId();
            hex_converter >> rec_id;

            if (m_dbase.SelectRecord(rec_id, rec))
            {
                BLITZ_LOG_INFO("Session id: %s is present in DB", session->getResouceId().c_str());
                if (!rec.ip_address.empty())
                {
                    BLITZ_LOG_WARNING("Session id: %s is already connected", session->getResouceId().c_str());
                    session->sendHeaders(VOD_STATEREJECT); // TODO must close connection
                }
                else
                {
                    it = m_filenames.find(session->getResouceId());
                    if (it != m_filenames.end())
                    {
                        session->openFile(it->second);
                        session->sendHeaders(VOD_STATE_OK);
                        rec.ip_address = session->getRemoteIP().c_str();
                        m_dbase.UpdateRecord(rec);
                    }
                    else
                    {
                        BLITZ_LOG_WARNING("No such resource found");
                        session->sendHeaders(VOD_STATEREJECT); // TODO must close connection
                        m_dbase.DeleteRecord(rec_id);
                    }
                }
            }
            else
            {
                BLITZ_LOG_WARNING("No such record in DB");
                session->sendHeaders(VOD_STATEREJECT); // TODO must close connection
            }
        }
        else if (VOD_STATE_CLOSED == sess_state)
        {
            // remove record from DB
            if (!session->getResouceId().empty())
            {
                blitz::record_id rec_id;
                MediaSessionDB::Record rec;
                std::stringstream hex_converter;

                hex_converter << std::hex << session->getResouceId();
                hex_converter >> rec_id;
                m_dbase.DeleteRecord(rec_id);
                m_filenames.erase(session->getResouceId());
            }

            BLITZ_LOG_WARNING("session closed");

            // remove connection from pool
            m_conn_pool.erase(session);

            // set to be destroyed later
            m_orphane_sessions.push_back(session);
        }
    }
}

bool VODService::execCommand(Command& cmd, std::string& response_out)
{
    unsigned command_id = 0;

    if (cmd.module().compare("vodmedia") != 0)
    {
        return false;
    }

    try
    {
        command_id = boost::lexical_cast<unsigned>(cmd.getParameterValue("cmd"));
    }
    catch (boost::bad_lexical_cast &)
    {
        BLITZ_LOG_ERROR("bad lexical cast for cmd parameter");
        return false;
    }

    response_out.clear();

    if (VCreateSeesionID == command_id)
    {
        blitz::FileReader file_reader;
        unsigned uid;
        MediaSessionDB::Record rec;
        std::stringstream file_stream_id;
        rec.id = time(NULL);

        try
        {
            uid = boost::lexical_cast<unsigned>(cmd.getParameterValue("uid"));
        }
        catch (boost::bad_lexical_cast &)
        {
            BLITZ_LOG_ERROR("bad lexical cast of uid parameter");
            return false;
        }

        std::string filename = cmd.getParameterValue("fname");
        if (filename.empty())
        {
            return false;
        }

        filename = this->m_basepath + filename;
        if (!file_reader.open(filename.c_str()))
        {
            BLITZ_LOG_ERROR("Error opening file: %s", filename.c_str());
            return false;
        }

        rec.id += uid + file_reader.getSize();
        file_reader.close();

        file_stream_id << std::hex << rec.id;
        BLITZ_LOG_INFO("session id: %s, %lu", file_stream_id.str().c_str(), rec.id);

        response_out = "http://" + m_ip_address + ":" + boost::lexical_cast<std::string>(m_port) + "/" + file_stream_id.str();

        rec.livetime_sec = 10800; // 4 hours
        m_dbase.InsertRecord(rec);
        m_filenames.insert( std::pair<std::string, std::string>(file_stream_id.str(), filename) );

        return true;
    }
    else if (VGetConnectedPeers == command_id)
    {
        std::size_t sess_count = m_dbase.GetRecordCount();

        for (std::size_t idx = 0; idx < sess_count; idx++)
        {
            blitz::record_id rec_id = m_dbase.GetRecordId(idx);
            MediaSessionDB::Record rec_out;

            if (!m_dbase.SelectRecord(rec_id, rec_out)) {
                return false;
            }
            response_out.clear();

            std::stringstream stream_id;
            stream_id << std::hex << rec_out.id;

            std::map<std::string ,std::string>::iterator it = m_filenames.find(stream_id.str());
            if (it != m_filenames.end())
            {
                response_out += it->second + "," + rec_out.ip_address + "," + stream_id.str() + "," +
                                boost::lexical_cast<std::string>(rec_out.livetime_sec) + ";";
            }
            return true;
        }
    }
    return false;
}

void VODService::clearSessions(void)
{
    if (!m_orphane_sessions.empty())
    {
        for (std::vector<VODSession*>::iterator it = m_orphane_sessions.begin();
                                                    it != m_orphane_sessions.end(); ++it)
        {
            VODSession* session = *it;
            delete session;
        }
        m_orphane_sessions.clear();
    }
}

VODSession::VODSession(boost::asio::io_service& io_service)
: TCPConnection(io_service), m_state(VOD_STATE_CLOSED), m_io_control_timer(io_service)
{}


void VODSession::handleReadHeader(const boost::system::error_code& error)
{
    m_io_control_timer.cancel();

    if (!error)
    {
        HTTPParser parser;
        std::string response; // HTTP response headers

        if (parser.isValid(m_client_request, HTTPReq))
        {
            HTTPRequest& req = parser.getRequest();
            std::string req_type = req.getRequestType();
            if ("GET" == req_type)
            {
                m_resource_id = req.getRequestedRes();
            }
            else
            {
                BLITZ_LOG_WARNING("not a GET request: %s", req_type.c_str());
                close();
                return;
            }
        }
        else
        {
            BLITZ_LOG_WARNING("Not a valid HTTP request");
            close();
            return;
        }

        m_state = VOD_STATE_REQ_RECEIVED;

        // notify our observers about new request
        notify();
    }
    else if (boost::asio::error::operation_aborted == error)
    {
        BLITZ_LOG_WARNING("Timeout sending data to connection");
    }
    else // error
    {
        BLITZ_LOG_ERROR("MediaHTTPConnection::handleReadHeader %s", error.message().c_str());
        close();
    }
}

void VODSession::handleWriteContent(const boost::system::error_code& error)
{
    //m_io_control_timer.cancel(); Don't need this, user can pause player

    if (!error)
    {
        int bytes_read = m_freader.read(m_packet.data(), DataPacket::max_size);

        if (bytes_read != -1)
        {
            m_packet.size(bytes_read);
            m_source.addData(&m_packet);
            if (m_freader.is_eof())
            {
                BLITZ_LOG_WARNING("Got EOF bit set from file");
                m_freader.close();
            }
        }
        else
        {
            close();
        }
    }
    else if (boost::asio::error::operation_aborted == error)
    {
        BLITZ_LOG_WARNING("Timeout sending data to connection");
    }
    else
    {
        BLITZ_LOG_ERROR("Terminating connection with error: %s", error.message().c_str());
        close();
    }
}

void VODSession::handleWriteReject(const boost::system::error_code& error)
{
    if (error)
    {
        BLITZ_LOG_ERROR("%s", error.message().c_str());
    }
    // Disable DB lookups in service
    m_resource_id.clear();
    close();
}

void VODSession::handleTimeoutSend(const boost::system::error_code& error)
{
    if (!error)
    {
        BLITZ_LOG_WARNING("Timeout sending data over connection closing ...");
        close();
    }
}

void VODSession::handleTimeoutReceive(const boost::system::error_code& error)
{
    if (!error)
    {
        BLITZ_LOG_WARNING("Timeout receiving data closing VOD session");
        close();
    }
}

void VODSession::start(void)
{
    TCPConnection::start();

    // start VOD session
    if (isConnected() && socket().is_open())
    {
        m_state = VOD_STATE_OPEN;

        BLITZ_LOG_INFO("got connection from: %s, from port:%d", getRemoteIP().c_str(), getRemotePort());

        // Set a deadline for receiving HTTP headers.
        m_io_control_timer.expires_from_now(boost::posix_time::seconds(VODSession::receive_timeout));

        boost::asio::async_read_until(socket(), m_client_request, "\r\n\r\n",
                                          boost::bind(&VODSession::handleReadHeader, this,
                                             boost::asio::placeholders::error));

        m_io_control_timer.async_wait(boost::bind(&VODSession::handleTimeoutReceive, this,
                                            boost::asio::placeholders::error));
    }
}

void VODSession::openFile(std::string& filename)
{
    if (!filename.empty())
    {
        m_filename = filename;
        if (!m_freader.open(m_filename.c_str()))
        {
            BLITZ_LOG_ERROR("Error opening file: %s", m_filename.c_str());
            return;
        }

        m_source.addSink(this);
    }
}

void VODSession::close(void)
{
    TCPConnection::close();

    m_state = VOD_STATE_CLOSED;

    m_freader.close();

    BLITZ_LOG_INFO("Closing");

    // notify our observers about closing;
    notify();
}

void VODSession::write(blitz::DataPacket* p)
{
    //m_io_control_timer.expires_from_now(boost::posix_time::seconds(VODSession::send_timeout));

    boost::asio::async_write(socket(), boost::asio::buffer(p->data(), p->size()),
                              boost::bind(&VODSession::handleWriteContent, this,
                                boost::asio::placeholders::error));
    /*
    m_io_control_timer.async_wait(boost::bind(&VODSession::handleTimeoutSend, this,
                                    boost::asio::placeholders::error));
    */
}

void VODSession::sendHeaders(VODResponse response_type)
{
    HTTPResponse http_response;

    if (response_type == VOD_STATEREJECT)
    {
        http_response.data(404);

        boost::asio::async_write(socket(),
                                 boost::asio::buffer(http_response.getHeaders().c_str(), http_response.getHeaders().size()),
                                   boost::bind(&VODSession::handleWriteReject, this,
                                     boost::asio::placeholders::error));
    }
    else if (response_type == VOD_STATE_OK)
    {
        http_response.stream(m_filename, m_freader.getSize());

        /* Dump headers when needed
        BLITZ_LOG_INFO("Headers : \n %s", http_response.getHeaders().c_str());
        */

        // send the HTTP headers for response
        boost::asio::async_write(socket(),
                                 boost::asio::buffer(http_response.getHeaders().c_str(), http_response.getHeaders().size()),
                                    boost::bind(&VODSession::handleWriteContent, this,
                                       boost::asio::placeholders::error));
    }
}

} // blitz
