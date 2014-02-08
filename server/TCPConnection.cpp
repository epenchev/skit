/*
 * TCPConnection.cpp
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
 *
 *  Created on: Aug 27, 2013
 *      Author: emo
 */

#include "server/TCPConnection.h"
#include "system/Task.h"
#include "system/TaskThread.h"
#include "utils/IDGenerator.h"

//#define LOG_DISABLE
#include "utils/Logger.h"

TCPConnection::TCPConnection(unsigned id, TCPSocket* inSocket)
 : m_id(id), m_socket(inSocket)
{}

TCPConnection::~TCPConnection()
{
    m_socket->Close();
    if (!m_channels.empty())
    {
        for (std::map<unsigned, IOChannel*>::iterator it = m_channels.begin(); it != m_channels.end(); ++it)
        {
            IOChannel* channel = it->second;
            delete channel;
        }
        m_channels.clear();
    }
    delete m_socket;
}

IOChannel* TCPConnection::OpenChannel(IOChannelListener* inListener)
{
    SystemMutexLocker lock(m_channelsLock);
    IOChannel* chan = NULL;
    if (inListener)
    {
        LOG(logDEBUG) << "Open channel and add listener";
        chan = new IOChannel(IDGenerator::Instance().Next(), inListener, this);
        m_channels.insert( std::pair<unsigned, IOChannel*>(chan->GetID(), chan) );
    }
    return chan;
}

void TCPConnection::CloseChannel(IOChannel* channel)
{
    std::list<ChannelEvent>::iterator iter;

    SystemMutexLocker lock(m_channelsLock);
    if (channel)
    {
        if (m_channels.count(channel->GetID()) > 0)
        {
            m_channels.erase(channel->GetID());

            SystemMutexLocker lockread(m_readLock);
            iter = m_readChanEvents.begin();
            // remove pending read events for this channel
            while (iter != m_readChanEvents.end())
            {
                if ((*iter).chanid == channel->GetID())
                {
                    iter = m_readChanEvents.erase(iter);
                }
            }

            SystemMutexLocker lockwrite(m_writeLock);
            iter = m_writeChanEvents.begin();
            // remove pending write events for this channel
            while (iter != m_writeChanEvents.end())
            {
                if ((*iter).chanid == channel->GetID())
                {
                    iter = m_writeChanEvents.erase(iter);
                }
            }
            delete channel;
        }
        else
        {
            LOG(logDEBUG) << "Channel not from connection";
        }
    }
}

IOChannel* TCPConnection::GetChannel(unsigned id)
{
    IOChannel* channel = NULL;
    if (id)
    {
        SystemMutexLocker lock(m_channelsLock);
        if (m_channels.count(id) > 0)
        {
            channel = m_channels.at(id);
        }
        else
        {
            LOG(logWARNING) << "No such channel present with this id:" << id;
        }
    }
    return channel;
}

void TCPConnection::Notify(IOEvent event, Buffer* data, IOChannel* channel)
{
    ChannelEvent chanEvent;
    if (channel)
    {
        // check if channel is from this connection
        if (m_channels.count(channel->GetID()) > 0)
        {
            if (data)
            {
                chanEvent.data = data;
                chanEvent.ioevent = event;
                chanEvent.chanid = channel->GetID();
                if (IOWrite == event)
                {
                    SystemMutexLocker lock(m_writeLock);
                    m_writeChanEvents.push_back(chanEvent);
                    if (!m_writeBusy)
                    {
                        m_writeBusy = true;
                        m_socket->Send(*data, this);
                    }
                }
                else // IORead , IOReadSome
                {
                    SystemMutexLocker lock(m_readLock);
                    m_readChanEvents.push_back(chanEvent);
                    if (!m_readBusy)
                    {
                        m_readBusy = true;
                        if (IORead == event)
                        {
                            m_socket->Receive(*data, this);
                        }
                        else if (IOReadSome == event)
                        {
                            m_socket->ReceiveSome(*data, this);
                        }
                    }
                }
            }
        }
    }
}

void TCPConnection::Disconnect()
{
    if (IsConnected())
    {
        m_socket->Close();

        Task* evtask = new Task();
        evtask->Connect(&TCPConnection::NotifyDisconnected, this );
        TaskThreadPool::Signal(evtask);
    }
}

void TCPConnection::NotifyDisconnected()
{
	LOG(logDEBUG) << "Notify listeners for OnConnectionClose() event connection:" << m_id;
	std::set<NetConnectionListener*> listeners = m_listeners;
	for (std::set<NetConnectionListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
	{
		(*it)->OnConnectionClose(*this);
	}
}

bool TCPConnection::IsConnected() const
{
    if (m_socket)
    {
        return m_socket->IsOpen();
    }
    return false;
}

std::string TCPConnection::GetRemoteAddress() const
{
    std::string address;
    if (m_socket->IsOpen())
    {
        ErrorCode outErr;
        address = m_socket->GetRemoteIP(outErr);
        if (outErr)
        {
            LOG(logERROR) << outErr.GetErrorMessage();
        }
    }
    return address;

}

unsigned TCPConnection::GetRemotePort() const
{
    unsigned port;
    if (m_socket->IsOpen())
    {
        ErrorCode outErr;
        port = m_socket->GetRemotePort(outErr);
        if (outErr)
        {
            LOG(logERROR) << outErr.GetErrorMessage();
        }
    }
    return port;
}

void TCPConnection::OnSend(TCPSocket* inSocket, std::size_t sendBytes, ErrorCode& inError)
{
    if (inSocket)
    {
        IOChannel* channel = NULL;

        m_writeLock.Lock();
        ChannelEvent chanEvent = m_writeChanEvents.front();
        m_writeChanEvents.pop_front();
        if (m_channels.count(chanEvent.chanid) > 0)
        {
            channel = m_channels.at(chanEvent.chanid);
        }

        // check if we have a pending write event
        if (!m_writeChanEvents.empty())
        {
            chanEvent = m_writeChanEvents.front();
            m_socket->Send(*chanEvent.data, this);
        }
        else
        {
            m_writeBusy = false;
        }
        m_writeLock.Unlock();

        if (channel)
        {
            channel->Notify(IOWrite, inError, sendBytes);
        }
    }
}

void TCPConnection::OnReceive(TCPSocket* inSocket, std::size_t receivedBytes, ErrorCode& inError)
{
    if (inSocket)
    {
        IOChannel* channel = NULL;

        m_readLock.Lock();
        ChannelEvent chanEvent = m_readChanEvents.front();
        if (m_channels.count(chanEvent.chanid) > 0)
        {
            channel = m_channels.at(chanEvent.chanid);
        }
        m_readChanEvents.pop_front(); // remove channel event no longer needed

        // check if we have a pending read event
        if (!m_readChanEvents.empty())
        {
            chanEvent = m_readChanEvents.front();
            if (IORead == chanEvent.ioevent)
            {
                m_socket->Receive(*chanEvent.data, this);
            }
            else if (IOReadSome == chanEvent.ioevent)
            {
                m_socket->ReceiveSome(*chanEvent.data, this);
            }
        }
        else
        {
            m_readBusy = false;
        }
        m_readLock.Unlock();

        if (channel)
        {
            channel->Notify(IORead, inError, receivedBytes);
        }
    }
}

void TCPConnection::AddListener(NetConnectionListener* listener)
{
    if (listener)
    {
        SystemMutexLocker lock(m_lockListeners);
        m_listeners.insert(listener);
    }
}

void TCPConnection::RemoveListener(NetConnectionListener* listener)
{
    SystemMutexLocker lock(m_lockListeners);
    std::set<NetConnectionListener*>::iterator it = m_listeners.find(listener);
    if (it != m_listeners.end())
    {
        m_listeners.erase(it);
    }
    else
    {
        LOG(logWARNING) << "Listener not from this connection";
    }
}


