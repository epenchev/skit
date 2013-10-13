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

#include "stream/Stream.h"
#include <cstddef>

Stream::Stream(unsigned streamId)
 : mstreamId(streamId), msource(NULL), mfilter(NULL), msink(NULL)
{}

Stream::~Stream()
{

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
	}
}

void Stream::SetSink(StreamSink* sink)
{
	if (sink)
	{
		this->msink = sink;
	}
}

void Stream::AddClient(StreamClient* client)
{

}

void Stream::RemoveClient(StreamClient* client)
{

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


