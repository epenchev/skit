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
#include "system/SystemThread.h"
#include "server/NetConnection.h"
#include <set>
#include <map>
#include <string>

class Stream
 : public SourceObserver,
   public FilterObserver,
   public SinkObserver
 //  public IOChannelObserver
{
public:
    Stream(unsigned streamId);
    virtual ~Stream();
    void Play();
    void Pause();
    void Stop();
    void Seek(unsigned position);
    void SetSource(StreamSource* source);
    void SetFilter(StreamFilter* filter);
    void SetSink(StreamSink* sink);
    void AddClient(StreamClient* client);
    void RemoveClient(StreamClient* client);
    void SetName(const char* name) { m_name = name; }
    std::string GetName() { return m_name; }
    unsigned GetStreamId();
    StreamSource* GetSource();
    StreamFilter* GetFilter();
    StreamSink*   GetSink();

protected:
    void WriteSink(Buffer* data);

    /* from SourceObserver */
    void OnStart(StreamSource* source);

    /* from SourceObserver */
    void OnStop(StreamSource* source);

    /* from SourceObserver */
    void OnDataReceive(StreamSource* source, Buffer* data, ErrorCode* error);

    /* from FilterObserver */
    void OnDataReady(StreamFilter* filter, Buffer* data);

    /* from SinkObserver */
    void OnDataOut(StreamClient* client, Buffer* data);

    /* from IOChannelObserver */
    void OnRead(IOChannel* chan, std::size_t bytesRead, ErrorCode* inErr);

    /* from IOChannelObserver*/
    void OnWrite(IOChannel* chan, std::size_t bytesWritten, ErrorCode* inErr);

    /* from IOChannelObserver */
    void OnConnectionClose(IOChannel* chan);
private:
    bool                         m_started;
    unsigned                     m_streamid;
    SystemMutex                  m_mutexLockOutput; // todo
    StreamSource*                m_source;
    StreamFilter*                m_filter;
    StreamSink*                  m_sink;
    std::string                  m_name;
    std::set<StreamClient*>      m_clients;
    // client id -> IOChan
    std::map<unsigned, IOChannel*> m_iochannels;  // todo thread safety
};

#endif /* STREAM_H_ */
