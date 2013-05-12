/*
 * HttpSink.cpp
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
 *  Created on: Jan 7, 2013
 *      Author: emo
 */

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <ctime>
#include "HttpSink.h"
#include "DataPacket.h"
#include "Log.h"

namespace blitz {

HttpSink::HttpSink(boost::asio::io_service& io_service, short port, std::string& ip_address, std::string& name, unsigned short conf_id)
 : m_session_timer(io_service), server(io_service, port, m_dbase), m_name(name), m_id(conf_id), m_port(port), m_ip_address(ip_address)
{
    server.start();

    m_session_timer.expires_from_now(boost::posix_time::seconds(HttpSink::session_loop_timeout));

    m_session_timer.async_wait(boost::bind(&HttpSink::sessionloop, this,
                                   boost::asio::placeholders::error));
}

void HttpSink::write(DataPacket* pkt)
{
    server.sendPacket(pkt);
}

bool HttpSink::execCommand(Command& cmd, std::string& response_out)
{
    std::string name;
    unsigned command_id = 0;

    if (cmd.module().compare("livetv") != 0)
    {
        return false;
    }

    try
    {
        command_id = boost::lexical_cast<unsigned>(cmd.getParameterValue("cmd"));
        name = cmd.getParameterValue("name");
    }
    catch (boost::bad_lexical_cast &)
    {
        BLITZ_LOG_ERROR("bad lexical cast %s", cmd.getParameterValue("cmd").c_str());
        return false;
    }

    response_out.clear();

    if (CreateSeesionID == command_id)
    {
        MediaSessionDB::Record rec;

        rec.id = time(NULL);

        try
        {
            std::stringstream stream_id;

            unsigned uid = boost::lexical_cast<unsigned>(cmd.getParameterValue("uid"));
            unsigned chid = boost::lexical_cast<unsigned>(cmd.getParameterValue("chid"));

            // channel chid from command must be equal with configuration id
            if (chid == m_id)
            {
                std::string net_port = boost::lexical_cast<std::string>(m_port);
                rec.id += uid + chid;

                stream_id << std::hex << rec.id;
                BLITZ_LOG_INFO("session id: %s, %lu", stream_id.str().c_str(), rec.id);
                response_out = "http://" + m_ip_address + ":" + net_port + "/" + stream_id.str();
            }
            else
            {
                return false;
            }
        }
        catch (boost::bad_lexical_cast &)
        {
            BLITZ_LOG_ERROR("bad lexical cast of uid and chid");
            return false;
        }
        BLITZ_LOG_INFO("rec.id %lu", rec.id);

        rec.livetime_sec = 10800; // 4 hours

        m_dbase.InsertRecord(rec);

        return true;
    }
    else if (GetConnectionCount == command_id)
    {
        unsigned connection_count = 0;

        try
        {
            connection_count = server.getConnectionCount();
            response_out = boost::lexical_cast<std::string>(connection_count);
        }
        catch(boost::bad_lexical_cast &)
        {
            BLITZ_LOG_ERROR("bad lexical cast %d", connection_count);
            return false;
        }
        return true;
    }
    else if (GetConnectedPeers == command_id)
    {
        //response_out = server.getConnectedPeers();
        unsigned chid = 0;
        std::size_t sess_count = m_dbase.GetRecordCount();

        try
        {
           chid = boost::lexical_cast<unsigned>(cmd.getParameterValue("chid"));
        }
        catch(boost::bad_lexical_cast &)
        {
            BLITZ_LOG_ERROR("bad lexical cast");
            return false;
        }

        if (chid == m_id)
        {
            response_out.clear();
            for (std::size_t idx = 0; idx < sess_count; idx++)
            {
                blitz::record_id rec_id = m_dbase.GetRecordId(idx);
                MediaSessionDB::Record rec_out;

                if (!m_dbase.SelectRecord(rec_id, rec_out)) {
                    break;
                }

                std::stringstream stream_id;
                stream_id << std::hex << rec_out.id;
                response_out += m_name + "," + rec_out.ip_address + "," + stream_id.str() + "," +
                                  boost::lexical_cast<std::string>(rec_out.livetime_sec) + ";";

            }
            return true;
        }
        else {
            return false;
        }
    }

    return false;
}

void HttpSink::sessionloop(const boost::system::error_code& error)
{
    if (error)
    {
        BLITZ_LOG_ERROR("%s",error.message().c_str());
        return;
    }

    std::size_t sess_count = m_dbase.GetRecordCount();

    for (std::size_t idx = 0; idx < sess_count; idx++)
    {
        blitz::record_id rec_id = m_dbase.GetRecordId(idx);
        MediaSessionDB::Record rec_out;

        if (!m_dbase.SelectRecord(rec_id, rec_out)) {
            break;
        }

        if (!rec_out.livetime_sec)
        {
            if (!rec_out.ip_address.empty()) // session is connected close it
            {
                BLITZ_LOG_INFO("Closing session");
                server.closeSession(rec_id);
                BLITZ_LOG_INFO("Session closed");
            }
            BLITZ_LOG_INFO("Deleting session from DB");
            m_dbase.DeleteRecord(rec_id);
            BLITZ_LOG_INFO("Session deleted from DB");
        }
        else
        {
            // reduce live time with 1 second
            rec_out.livetime_sec -= 1;
            m_dbase.UpdateRecord(rec_out);
        }
    }

    m_session_timer.expires_from_now(boost::posix_time::seconds(HttpSink::session_loop_timeout));
    m_session_timer.async_wait(boost::bind(&HttpSink::sessionloop, this,
                                             boost::asio::placeholders::error));
}

} // blitz



