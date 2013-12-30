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
 : m_clientID(1), m_creationTime(0), m_subscribedStreamID(0)
{}

StreamClient::~StreamClient()
{}

unsigned long StreamClient::GetCreationTime()
{
    return m_creationTime;
}

void StreamClient::Subscribe(Stream* s)
{
    if (s)
    {
        s->AddClient(this);
        m_subscribedStreamID = s->GetStreamID();
        //LOG(logINFO) << "Subscribed to stream:" << s->GetStreamID() << " " << s->GetName();
    }
}

void StreamClient::UnSubscribe()
{
	if (IsSubscribed())
	{
		/* TODO
		StreamFactory::GetStream(m_subscribedStreamID);
        if (s)
        {
            s->RemoveClient(this);
            LOG(logINFO) << "remove from stream ";
        }
        */
    }
	else
	{
		LOG(logINFO) << "Client is not subscribed";
	}
}

void StreamClient::Register(NetConnection* conn)
{
    if (conn)
    {
        m_connections.insert(conn);
    }
}

std::set<NetConnection*>& StreamClient::GetConnections()
{
    return m_connections;
}

unsigned StreamClient::IsSubscribed()
{
    return m_subscribedStreamID;
}

unsigned StreamClient::GetID()
{
    return m_clientID;
}


