/*
 * Stream.cpp
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
 *  Created on: Oct 12, 2013
 *      Author: emo
 */

#include <cstddef>
#include <ctime>
#include "stream/Stream.h"
#include "stream/StreamPlayItem.h"
#include "stream/StreamPacketRec.h"
#include "stream/StreamFactory.h"
#include "utils/ErrorCode.h"

//#define LOG_DISABLE
#include "utils/Logger.h"

#include "utils/IDGenerator.h"
#include "system/Task.h"
#include "system/TaskThread.h"

Stream::Stream(unsigned streamID, StreamSource* source, StreamFilter* filter, StreamSink* sink)
 : m_streamID(streamID), m_source(source), m_filter(filter), m_sink(sink)
{}

Stream::~Stream()
{
    if (m_source)
    {
        m_source->RemoveListener(this);
    }
    if (m_filter)
    {
        m_filter->RemoveListener(this);
    }
}

void Stream::AddListener(StreamListener* listener)
{
    LOG(logDEBUG) << "Add a listener to stream:" << m_streamID
                  << " " << StreamFactory::GetStreamName(m_streamID);
    if (listener)
    {
        SystemMutexLocker lock(m_lockListeners);
        m_listeners.insert(listener);
        LOG(logDEBUG) << "Listener added to stream:" << m_streamID
                      << " " << StreamFactory::GetStreamName(m_streamID);
    }
}

void Stream::RemoveListener(StreamListener* listener)
{
    LOG(logDEBUG) << "Remove a listener from stream:" << m_streamID
                  << " " << StreamFactory::GetStreamName(m_streamID);
    if (listener)
    {
        SystemMutexLocker lock(m_lockListeners);
        if (m_listeners.count(listener) > 0)
        {
            m_listeners.erase(listener);
            LOG(logDEBUG) << "Listener removed from stream:" << m_streamID
                          << " " << StreamFactory::GetStreamName(m_streamID);
        }
    }
}

// TODO throw exception
void Stream::Play()
{
    if (StreamFactory::IsPublished(m_streamID))
    {
        m_streamName = StreamFactory::GetStreamName(m_streamID);
        LOG(logDEBUG) << "Starting stream:" << m_streamID << " " << m_streamName;
        if (m_source && m_sink)
        {
            m_source->AddListener(this);
            m_source->Start(*this);
            m_sink->Start(*this);
            if (m_filter)
            {
                m_filter->AddListener(this);
                m_filter->Start(*this);
            }
            LOG(logINFO) << "Stream: " << StreamFactory::GetStreamName(m_streamID) << " started";
        }
        else
        {
            LOG(logERROR) << "Stream:" << m_streamID << " " << m_streamName
                          << " missing source or sink, unable to play";
        }
    }
    else
    {
        LOG(logWARNING) << "Stream " << m_streamID << " " << m_streamName
                        << " is not published can't play, must publish first";
    }
}

void Stream::Pause()
{
    LOG(logDEBUG) << "Pause stream:" << m_streamName;
    if (m_source)
    {
        m_source->Stop();
        LOG(logINFO) << "Stream:" << m_streamName << " paused";
    }
}

void Stream::Stop()
{
    LOG(logDEBUG) << "Stop stream:" << m_streamName;
    if (m_source)
    {
        m_source->Stop();
        LOG(logINFO) << "Stream:" << m_streamName << " stopped";
    }
}

void Stream::Seek(StreamPlayItem& playItem)
{
    if (!m_streamName.empty())
    {
    	if (playItem.GetStreamID() == m_streamID)
    	{
    		if (m_source->IsSeekable())
    		{
    			m_lockSource.Lock();
    			m_source->Seek(playItem);
    		}
    		else
    		{
    			LOG(logERROR) << "Stream " << m_streamName << "is not seek-able";
    		}
    	}
    	else
    	{
    		LOG(logERROR) << "Stream:" << m_streamName << " playItem not for this stream";
    	}
    }
    else
    {
        LOG(logERROR) << "Stream is not started";
    }
}

// TODO throw exception
void Stream::AddClient(StreamClientPtr client)
{
    if (client)
    {
    	SystemMutexLocker lock(m_lockClients);
        if (m_clients.count(client) != 0)
        {
            LOG(logWARNING) << "client:" << client->GetID() << " already present in the stream "
                            << m_streamName;
        }
        else
        {
        	LOG(logDEBUG) << "Add client to stream " << m_streamName;
        	m_clients.insert(client);
        	TaskThreadPool::AutoSignalTask(&Stream::NotifyOnClientAccepted, this, client);
        }
    }
}

// TODO throw exception
void Stream::RemoveClient(StreamClientPtr client)
{
    if (client)
    {
    	SystemMutexLocker lock(m_lockClients);
    	if (m_clients.count(client) > 0)
        {
    		LOG(logDEBUG) << "Remove client from stream " << m_streamName;
    		m_clients.erase(client);
    		TaskThreadPool::AutoSignalTask(&Stream::NotifyOnClientRemoved, this, client);
        }
    }
}

void Stream::OnStart(StreamSource& source, ErrorCode& error)
{
    LOG(logDEBUG) << "{}";
    if (error)
    {
        LOG(logERROR) << "Error starting source: " << error.Message();
    }
}

void Stream::OnStop(StreamSource& source)
{
    LOG(logINFO) << "StreamSource stopped";
}

void Stream::OnDataReceive(StreamSource& source, IStreamPacket& packet, ErrorCode& error)
{
	m_lockSource.Unlock();

    LOG(logDEBUG) << " Stream " << m_streamName << " OnDataReceive() event from source";
    if (!error)
    {
        if (m_filter)
        {
        	LOG(logDEBUG) << " Stream " << m_streamName << " write to filter";
            m_filter->WriteData(packet);
        }
        else
        {
        	LOG(logDEBUG) << " Stream " << m_streamName << " write to sink";
            m_sink->WriteData(packet);
        }
    }
    else
    {
        LOG(logERROR) << "Got error from source " << error.Message();
    }
}

void Stream::OnDataReady(StreamFilter& filter, IStreamPacket& data)
{
    LOG(logDEBUG1) << " Stream " << m_streamName << " OnDataReady() event from filter";
    LOG(logDEBUG1) << " Stream " << m_streamName << " write to sink";
    m_sink->WriteData(data);
}

void Stream::NotifyOnClientAccepted(StreamClientPtr client)
{
	m_lockListeners.Lock();
	std::set<StreamListener*> listeners = m_listeners;
	m_lockListeners.Unlock();

	LOG(logDEBUG) << "Notify listeners OnClientAccepted()";
	for (std::set<StreamListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it)
	{
		(*it)->OnClientAccepted(*this, client);
	}
}

void Stream::NotifyOnClientRemoved(StreamClientPtr client)
{
	m_lockListeners.Lock();
	std::set<StreamListener*> listeners = m_listeners;
	m_lockListeners.Unlock();

	LOG(logDEBUG) << "Notify listeners OnClientRemoved()";
	for (std::set<StreamListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it)
	{
		(*it)->OnClientRemoved(*this, client);
	}
}
