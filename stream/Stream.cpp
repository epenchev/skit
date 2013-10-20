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

Stream::Stream(unsigned streamId)
 : mstreamId(streamId), msource(NULL), mfilter(NULL), msink(NULL)
{}

Stream::~Stream()
{
    if (msource)
    {
        msource->RemoveListener(this);
    }

    if (mfilter)
    {
        mfilter->RemoveListener(this);
    }

    if (msink)
    {
        msink->RemoveListerner(this);
    }

    for (std::list<StreamClient*>::iterator it = mlistClients.begin(); it != mlistClients.end(); ++it)
    {
        StreamClient* client = *it;
        delete client;
    }
    mlistClients.clear();
}

void Stream::Play()
{
    if (msource)
    {
        msource->Start();
    }
}

void Stream::Pause()
{
    if (msource)
    {
        msource->Stop();
    }
}

void Stream::Stop()
{
    if (msource)
    {
        msource->Stop();
    }
}

void Stream::Seek()
{
    if (msource)
    {
        msource->Seek();
    }
}

void Stream::SetSource(StreamSource* source)
{
    if (source)
    {
        this->msource = source;
        msource->AddListener(this);
    }
}

void Stream::SetFilter(StreamFilter* filter)
{
    if (filter)
    {
        this->mfilter = filter;
        mfilter->AddListener(this);
    }
}

void Stream::SetSink(StreamSink* sink)
{
    if (sink)
    {
        this->msink = sink;
        msink->AddListerner(this);
    }
}

void Stream::AddClient(StreamClient* client)
{
    if (client)
    {
        mlistClients.push_back(client);
    }
}

void Stream::RemoveClient(StreamClient* client)
{
    if (!mlistClients.empty() && client)
    {
        for (std::list<StreamClient*>::iterator it = mlistClients.begin(); it != mlistClients.end(); ++it)
        {
            // = *it;
        }
    }
}

unsigned Stream::GetStreamId()
{
    return this->mstreamId;
}

StreamSource* Stream::GetSource()
{
    return this->msource;
}

StreamFilter* Stream::GetFilter()
{
    return this->mfilter;
}

StreamSink* Stream::GetSink()
{
    return this->msink;
}

void Stream::OnStart(StreamSource* source)
{

}

void Stream::OnStop(StreamSource* source)
{

}

void Stream::OnDataReceive(StreamSource* source, Buffer* data, ErrorCode* error)
{

}

void Stream::OnDataReady(StreamFilter* filter, Buffer* data)
{

}

void Stream::OnDataSent(StreamClient* client , StreamSink* sink, ErrorCode* error)
{

}



