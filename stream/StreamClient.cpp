/*
 * StreamClient.cpp
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
 *  Created on: Oct 31, 2013
 *      Author: emo
 */

#include "stream/StreamClient.h"
#include "stream/Stream.h"
#include "server/NetConnection.h"
#include "server/ServerController.h"
#include "Logger.h"
#include "system/Buffer.h"

StreamClient::StreamClient()
 : m_subscribed(false), m_clientId(1), m_creationTime(0)
{}

StreamClient::~StreamClient()
{}

unsigned long StreamClient::GetCreationTime()
{
    return m_creationTime;
}

void StreamClient::Subscribe(unsigned streamId)
{
    if (streamId)
    {
        Stream* s = ServerController::GetStream(streamId);
        if (s)
        {
            s->AddClient(this);
            m_subscribed = true;
            LOG(logINFO) << "Subscribed to stream " << s->GetName();
        }
        else
        {
            LOG(logINFO) << "Error getting stream with id:" << streamId;
        }
    }
}

void StreamClient::UnSubscribe(unsigned streamId)
{
    if (streamId)
    {
        Stream* s = ServerController::GetStream(streamId);
        if (s)
        {
            s->RemoveClient(this);
            m_subscribed = false;
            LOG(logINFO) << "remove from stream " << s->GetName();
        }
        else
        {
            LOG(logINFO) << "Error getting stream with id:" << streamId;
        }
    }
}

void StreamClient::Write(BufferPtr data)
{

}

void StreamClient::Register(NetConnection* conn)
{
    if (conn)
    {
        m_connections.insert(conn);
    }
}

void StreamClient::Unregister(NetConnection* conn)
{
    if (conn)
    {
        m_connections.erase(conn);
    }
}

std::set<NetConnection*>& StreamClient::GetConnections()
{
    return m_connections;
}

bool StreamClient::IsSubscribed()
{
    return m_subscribed;
}

void StreamClient::GetProperties()
{

}

unsigned StreamClient::GetId()
{
    return m_clientId;
}

void StreamClient::Disconnect()
{
    // todo for test
    NetConnection* conn = *m_connections.begin();
    conn->Disconnect();
}

