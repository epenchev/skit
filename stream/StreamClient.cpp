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
 *  Created on: Jan 13, 2014
 *      Author: emo
 */

#include "utils/Logger.h"
#include "utils/IDGenerator.h"
#include "stream/StreamClient.h"

StreamClient::StreamClient()
 : m_clientID(IDGenerator::Instance().Next()), m_creationTime(time(NULL))
{}

StreamClient::~StreamClient()
{}

unsigned long StreamClient::GetCreationTime()
{
    return m_creationTime;
}

PropertyMap& StreamClient::GetProperties()
{
    return m_propetries;
}

void StreamClient::Register(NetConnection* conn)
{
    if (conn)
    {
        SystemMutexLocker lock(m_lockConnections);
        if (m_connections.count(conn) == 0)
        {
            LOG(logDEBUG) << "Register connection to client";
            m_connections.insert(conn);
        }
    }
}

void StreamClient::UnRegister(NetConnection* conn)
{
    if (conn)
    {
        SystemMutexLocker lock(m_lockConnections);
        if (m_connections.count(conn) != 0)
        {
            LOG(logDEBUG) << "Remove connection from client";
            m_connections.erase(conn);
        }
    }
}

std::set<NetConnection*>& StreamClient::GetConnections()
{
    return m_connections;
}

unsigned StreamClient::GetID()
{
    return m_clientID;
}
