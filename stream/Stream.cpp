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
#include "stream/Stream.h"
#include "system/Buffer.h"
#include "ErrorCode.h"
#include "Logger.h"
#include "stream/StreamFactory.h"
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

void Stream::Play()
{
    LOG(logDEBUG) << "Start stream:" << m_streamID
    		      << " " << StreamFactory::GetStreamName(m_streamID);
	if (m_source)
    {
		if (StreamFactory::IsPublished(m_streamID))
		{
			m_source->Start();
			LOG(logINFO) << "Playing stream:" << StreamFactory::GetStreamName(m_streamID);
		}
    }
}

void Stream::Pause()
{
	LOG(logDEBUG) << "Pause stream:" << StreamFactory::GetStreamName(m_streamID);
    if (m_source)
    {
        m_source->Stop();
        LOG(logINFO) << "Stream:" << StreamFactory::GetStreamName(m_streamID) << " paused";
    }
}

void Stream::Stop()
{
	LOG(logDEBUG) << "Stop stream:" << StreamFactory::GetStreamName(m_streamID);
	if (m_source)
    {
        m_source->Stop();
        LOG(logINFO) << "Stream:" << StreamFactory::GetStreamName(m_streamID) << " stopped";
    }
}

void Stream::Seek(unsigned position)
{
    LOG(logDEBUG) << "Seeking into stream:" << m_streamID << " at position:" << position;
    if (m_source)
    {
        if (m_source->IsSeekable())
        {
            m_source->Seek(position);
            LOG(logDEBUG) << "Stream:" << m_streamID  << "set to position:" << position;
        }
    }
}

void Stream::AddClient(StreamClient* client)
{
	SystemMutexLocker lock(m_lockClients);
	LOG(logDEBUG) << "Add client to stream " << StreamFactory::GetStreamName(m_streamID);
    if (client)
    {
        if (m_clients.count(client) != 0)
        {
            LOG(logWARNING) << "client:" << client->GetID() << " already present in the stream "
            		        << StreamFactory::GetStreamName(m_streamID);
        }
        else
        {
        	m_clients.insert(client);
        	LOG(logDEBUG) << "Notify listeners OnClientSubscribed()";
        	SystemMutexLocker listenersLock(m_lockListeners);
        	for (std::set<StreamListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
        	{
        		Task* eventTask = new Task();
        		eventTask->Connect(&StreamListener::OnClientSubscribed, *it, boost::ref(*this), *client);
        		TaskThreadPool::Signal(eventTask);
        	}
        }
    }
}

void Stream::RemoveClient(StreamClient* client)
{
	SystemMutexLocker lock(m_lockClients);
	LOG(logDEBUG) << "Remove client from stream " << StreamFactory::GetStreamName(m_streamID);
    if (client)
    {
    	if (m_clients.count(client) > 0)
        {
    		m_clients.erase(client);
    		LOG(logDEBUG) << "Notify listeners OnClientUnSubscribed()";
    		SystemMutexLocker listenersLock(m_lockListeners);
    		for (std::set<StreamListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    		{
    			Task* eventTask = new Task();
    		    eventTask->Connect(&StreamListener::OnClientUnSubscribed, *it,  boost::ref(*this), *client);
    		    TaskThreadPool::Signal(eventTask);
    		}
        }
    }
}

void Stream::WriteSink(Buffer* data)
{
    if (data)
    {
        if (m_sink)
        {
            m_sink->WriteData(data, this);
        }
    }
}

void Stream::OnStart(StreamSource& source)
{
    LOG(logINFO) << "Stream started";
}

void Stream::OnStop(StreamSource& source)
{
    LOG(logINFO) << "Stream stopped";
}

void Stream::OnDataReceive(StreamSource& source, Buffer* data, ErrorCode& error)
{
    if (!error)
    {
        if (data)
        {
            if (m_filter)
            {
                // give filter work
                m_filter->WriteData(data);
            }
            else
            {
                WriteSink(data);
            }
        }
        else
        {
            LOG(logERROR) << "Invalid data from source";
        }
    }
    else
    {
        LOG(logERROR) << error.GetErrorMessage();
    }
}

void Stream::OnDataReady(StreamFilter* filter, Buffer* data)
{
    if (data)
    {
        WriteSink(data);
    }
    else
    {
        LOG(logERROR) << "Invalid data from filter";
    }
}

