/*
 * VODMediaServer.cpp
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

#include "VODMediaServer.h"
#include "HttpConnSink.h"
#include "Subject.h"
#include "Log.h"
#include "FsSource.h"

namespace blitz {

VODMediaServer::VODMediaServer(boost::asio::io_service& io_service, const unsigned int tcp_port)
 : MediaHTTPServer(io_service, tcp_port)
{ m_source = NULL; }

TCPConnection* VODMediaServer::createTCPConnection(boost::asio::io_service& io_service)
{
    HTTPConnSink* connection = new HTTPConnSink(io_service);
    connection->attach(this);

    return connection;
}

void VODMediaServer::update(Subject* changed_subject)
{
    if (changed_subject)
    {
        HTTPConnSink* conn = NULL;
        try
        {
            conn = dynamic_cast<HTTPConnSink*>(changed_subject);
        }
        catch(std::exception& e)
        {
            BLITZ_LOG_ERROR("bad cast exception: %s", e.what());
            return;
        }

        MediaHTTPConnectionState conn_state = conn->getState();
        if (STATE_OPEN == conn_state)
        {
            // TODO attach ACL module here in the future
            // TODO set timer for connection.
            BLITZ_LOG_INFO("got connection from: %s from port: %d",
                                conn->getRemoteIP().to_string().c_str(), conn->getRemotePort());
        }
        else if (STATE_CLOSED == conn_state)
        {
            // TODO inform DB for connection close.
            BLITZ_LOG_INFO("got closing connection, connection pool count: %d", m_conn_pool.size() - 1);
        }
        else if (STATE_REQ_RECEIVED)
        {
            if (conn->isReady() && m_source)
            {
                m_source->readPacket();
                return;
            }


            // TODO check requested resource against a database
            BLITZ_LOG_INFO("resource is: %s", conn->getResouceId().c_str());

            if (!conn->getResouceId().empty())
            {
                m_source = new FsSource("/media/c85997a5-9a24-45f4-a65e-b35085707131/Movies/Safe.House.2012.DVDRiP.XviD.AC3-TODE/Safe.House.2012.DVDRiP.XviD.AC3-TODE.avi");
                m_source->addSink(conn);
                m_source->readPacket();
            }
        }
    }
}

} // blitz
