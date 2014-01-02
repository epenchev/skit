/*
 * IOChannel.cpp
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
 *  Created on: Nov 22, 2013
 *      Author: emo
 */

#include "server/IOChannel.h"
#include "utils/Logger.h"

IOChannel::IOChannel(unsigned id, IOChannelListener* listener, NetConnection* conn)
 : m_channelId(id), m_writebuff(NULL), m_readbuff(NULL), m_connection(conn), m_listener(listener)
{}

IOChannel::~IOChannel()
{}

void IOChannel::Write(Buffer& data)
{
    if (m_connection)
    {
        if (m_writebuff)
        {
            LOG(logWARNING) << "Write operation not completed";
        }
        m_writelock.Lock();
        m_writebuff = &data;
        LOG(logDEBUG) << "Notify connection IOWrite ID:" << GetID();
        m_connection->Notify(IOWrite, m_writebuff, this);
        m_writelock.Unlock();
    }
    else
    {
        LOG(logERROR) << "Missing connection in chanId:" << GetID();
    }
}

void IOChannel::Read(Buffer& data, bool read_some)
{
    if (m_connection)
    {
        if (m_readbuff)
        {
            LOG(logWARNING) << "Read operation not completed";
        }
        m_readlock.Lock();
        m_readbuff = &data;
        if (read_some)
        {
            m_connection->Notify(IOReadSome, m_readbuff, this);
        }
        else
        {
            m_connection->Notify(IORead, m_readbuff, this);
        }
        m_readlock.Unlock();
    }
}

void IOChannel::Notify(IOEvent event, ErrorCode& err, std::size_t iobytes)
{
    if (!m_listener)
    {
        LOG(logERROR) << "Missing observer in chanId:" << GetID();
    }

    if ((IORead == event) || (IOReadSome == event))
    {
        m_readlock.Lock();
        m_readbuff = NULL;
        m_readlock.Unlock();
        if (m_listener)
        {
            m_listener->OnRead(this, iobytes, err);
        }
    }
    else if (IOWrite == event)
    {
        m_writelock.Lock();
        m_writebuff = NULL;
        m_writelock.Unlock();
        if (m_listener)
        {
            m_listener->OnWrite(this, iobytes, err);
        }
    }
}

