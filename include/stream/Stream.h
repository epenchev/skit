/*
 * Stream.h
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
 *  Created on: Oct 10, 2013
 *      Author: emo
 */

#ifndef STREAM_H_
#define STREAM_H_

#include "StreamSource.h"
#include "StreamFilter.h"
#include "StreamSink.h"
#include "StreamClient.h"
#include <list>

class Stream
 : public SourceObserver,
   public FilterObserver,
   public SinkObserver
{
public:
    Stream(unsigned streamId);
    virtual ~Stream();
    void Play();
    void Pause();
    void Stop();
    void Seek();
    void SetSource(StreamSource* source);
    void SetFilter(StreamFilter* filter);
    void SetSink(StreamSink* sink);
    void AddClient(StreamClient* client);
    void RemoveClient(StreamClient* client);
    unsigned GetStreamId();
    StreamSource* GetSource();
    StreamFilter* GetFilter();
    StreamSink*   GetSink();

protected:
    /* from SourceObserver */
    void OnStart(StreamSource* source);

    /* from SourceObserver */
    void OnStop(StreamSource* source);

    /* from SourceObserver */
    void OnDataReceive(StreamSource* source, Buffer* data, ErrorCode* error);

    /* from FilterObserver */
    void OnDataReady(StreamFilter* filter, Buffer* data);

    /* from SinkObserver */
    void OnDataSent(StreamClient* client , StreamSink* sink, ErrorCode* error);

private:
    unsigned                    mstreamId;
    StreamSource*                msource;
    StreamFilter*                mfilter;
    StreamSink*                  msink;
    std::list<StreamClient*>     mlistClients;
};

#endif /* STREAM_H_ */
