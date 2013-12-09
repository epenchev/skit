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

Stream::Stream(unsigned id)
 : m_started(false), m_streamid(id), m_source(NULL), m_filter(NULL), m_sink(NULL)
{
    //Log::ReportingLevel() = logERROR;
}

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
    if (m_sink)
    {
        m_sink->RemoveListerner(this);
    }
    for (std::set<StreamClient*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        StreamClient* client = *it;
        client->Disconnect();
    }
    m_clients.clear();
}

void Stream::Play()
{
    if (m_source)
    {
        if (!m_started)
        {
            LOG(logERROR) << "Playing";
            m_started = true;
            m_source->Start();
        }
    }
}

void Stream::Pause()
{
    if (m_source)
    {
        m_source->Stop();
    }
}

void Stream::Stop()
{
    if (m_source && m_started)
    {
        m_started = false;
        m_source->Stop();
    }
}

void Stream::Seek(unsigned position)
{
    LOG(logDEBUG) << "Seeking into stream:" << m_streamid << " at position:" << position;
    if (m_source)
    {
        if (m_source->IsSeekable())
        {
            m_source->Seek(position);
        }
        else
        {
            LOG(logWARNING) << "Stream is not seekable";
        }
    }
    else
    {
        LOG(logERROR) << "no source present in stream";
    }
}

void Stream::SetSource(StreamSource* source)
{
    if (source)
    {
        m_source = source;
        m_source->AddListener(this);
    }
    else
    {
        LOG(logERROR) << "Invalid source";
    }
}

void Stream::SetFilter(StreamFilter* filter)
{
    if (filter)
    {
        m_filter = filter;
        m_filter->AddListener(this);
    }
    else
    {
        LOG(logERROR) << "Invalid filter";
    }
}

void Stream::SetSink(StreamSink* sink)
{
    if (sink)
    {
        m_sink = sink;
        m_sink->AddListerner(this);
    }
    else
    {
        LOG(logERROR) << "Invalid sink";
    }
}

void Stream::AddClient(StreamClient* client)
{
    LOG(logERROR) << "client count:" << m_clients.size();
    if (client)
    {
        if (m_clients.count(client) != 0)
        {
            //LOG(logDEBUG) << "client:" << client->GetId() << " already present in the stream " << m_name;
        }
        else
        {
            //LOG(logDEBUG) << "Adding client:" << client->GetId() << " to stream: " << m_name;
            NetConnection* clientConn = *client->GetConnections().begin();
            if (!clientConn)
            {
                LOG(logWARNING) << "Invalid connection from client:" << client->GetId();
            }
            else
            {
                //LOG(logDEBUG) << "Opening channel " << clientConn->GetConnId();
                IOChannel* iochan; /*= clientConn->OpenChannel(this); */
                //LOG(logDEBUG) << "Channel opened " << iochan->GetChannelId();
                if (iochan)
                {
                    m_clients.insert(client);
                    //LOG(logDEBUG) << "Channel:" << iochan->GetChannelId()
                                         //       << " -> Connection:" << iochan->GetChannelId();
                    m_iochannels.insert(std::pair<unsigned, IOChannel*>(client->GetId(), iochan));
                    Play();
                }
                else
                {
                    LOG(logERROR) << "Error opening channel to connection" << client->GetId();
                }
            }
        }
    }
    else
    {
        LOG(logERROR) << "Invalid client";
    }
}

void Stream::RemoveClient(StreamClient* client)
{
    LOG(logERROR) << "client count:" << m_clients.size();
    if (!m_clients.empty())
    {
        if (client)
        {
            if (m_clients.count(client) > 0)
            {
                m_clients.erase(client);
            }
        }
        else
        {
            LOG(logERROR) << "Invalid client";
        }
    }
    else
    {
        LOG(logERROR) << "Invalid clients list in stream";
    }
}

unsigned Stream::GetStreamId()
{
    return m_streamid;
}

StreamSource* Stream::GetSource()
{
    return m_source;
}

StreamFilter* Stream::GetFilter()
{
    return m_filter;
}

StreamSink* Stream::GetSink()
{
    return m_sink;
}

void Stream::OnStart(StreamSource* source)
{
    if (m_source->IsSeekable())
    {
        LOG(logDEBUG) << "Source resumed";
    }
    else
    {
        LOG(logINFO) << "Stream started";
    }
}

void Stream::OnStop(StreamSource* source)
{
    LOG(logINFO) << "Stream stopped";
    for (std::set<StreamClient*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        StreamClient* client = *it;
        client->Disconnect();
        RemoveClient(client);
    }
}

void Stream::WriteSink(Buffer* data)
{
    if (data)
    {
        if (m_sink)
        {
            if (!m_clients.empty())
            {
                for (std::set<StreamClient*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
                {
                    StreamClient* client = *it;
                    m_sink->WriteData(data, client);
                }
            }
        }
        else
        {
            //LOG(logWARNING) << "No sink present in stream ID:" << m_streamid;
            // TODO add as separate function
            if (m_clients.empty())
            {
                LOG(logERROR) << "No clients subscribed to this stream:" << m_name;
                return;
            }

            LOG(logERROR) << "client cout:" << m_clients.size();
            for (std::set<StreamClient*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
            {
                StreamClient* client = *it;
                if (client)
                {
                    if (m_iochannels.count(client->GetId()) > 0)
                    {
                        ErrorCode err;
                        IOChannel* iochan = m_iochannels.at(client->GetId());
                        //iochan->Emit(data, IOWrite, err);
                    }
                }
                else
                {
                    LOG(logERROR) << "Invalid client in stream:" << m_name;
                }
            }

        }
    }
}

void Stream::OnDataReceive(StreamSource* source, Buffer* data, ErrorCode* error)
{
    ErrorCode err;
    err = *error;

    // TODO notify listener if present.
    if (!err)
    {
        if (data)
        {
            //LOG(logDEBUG) << "Got data from source with size:" << data->Size();
            // Ensure thread safety for filter and sink.
            m_mutexLockOutput.Lock();
            if (m_source->IsSeekable())
            {
                // VOD, play source after sink is done
                m_started = false;
            }
            if (m_filter)
            {
                m_filter->WriteData(data);
            }
            else
            {
                WriteSink(data);
            }
            m_mutexLockOutput.Unlock();
        }
        else
        {
            LOG(logERROR) << "Invalid data from source";
        }
    }
    else
    {
        LOG(logERROR) << err.GetErrorMessage();
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

void Stream::OnDataOut(StreamClient* client, Buffer* data)
{
    // if source is VOD resume read operation with Play()
    if (m_source->IsSeekable())
    {
        //Play();
    }

    if (!m_clients.empty())
    {
        for (std::set<StreamClient*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
        {
            StreamClient* client = *it;
            // TODO Get connections.
        }
    }
}

void Stream::OnWrite(IOChannel* chan, std::size_t bytesWritten, ErrorCode* inErr)
{
    ErrorCode err = *inErr;
    if (err)
    {
        LOG(logERROR) << err.GetErrorMessage();
        StreamClient* delcl = NULL;
        for (std::set<StreamClient*>::iterator it=m_clients.begin(); it!=m_clients.end(); ++it)
        {
            StreamClient* cl = *it;
            NetConnection* clientConn = *cl->GetConnections().begin();
            /*
            if (chan->GetConnId() == clientConn->GetID())
            {
                delcl = cl;
                LOG(logERROR) << "client removed";
            }
            */
        }
        RemoveClient(delcl);
        // TODO temp fix remove
        //StreamClient* client = *m_clients.begin();
        //client->Disconnect();
        //RemoveClient(client);


    }
    else
    {
        //LOG(logDEBUG) << "bytes written:" << bytesWritten;
        // if source is VOD resume read operation with Play()
        if (m_source->IsSeekable())
        {
            Play();
        }
    }
}

void Stream::OnConnectionClose(IOChannel* chan)
{
    LOG(logERROR) << "connection  close    ";
    StreamClient* delcl = NULL;
    for (std::set<StreamClient*>::iterator it=m_clients.begin(); it!=m_clients.end(); ++it)
    {
        StreamClient* cl = *it;
        NetConnection* clientConn = *cl->GetConnections().begin();

        /*
        if (chan->GetID() == clientConn->GetID())
        {
            delcl = cl;
            LOG(logERROR) << "Remove client";
        }
        */
    }
    RemoveClient(delcl);

}

