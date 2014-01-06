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

#include "stream/StreamSource.h"
#include "stream/StreamFilter.h"
#include "stream/StreamSink.h"
#include "system/SystemThread.h"
#include "utils/PropertyMap.h"
#include "server/NetConnection.h"
#include <set>

class Stream; // forward

/**
* Client is an abstraction representing user connected to Blitz media server.
* Clients are tied to connections and subscribed to Streams.
*/
class StreamClient
{
public:
    StreamClient();
    virtual ~StreamClient();

    /**
    * Associate connection with client.
    * No thread safety provided for this call.
    * @param conn - pointer to NetConnection object.
    */
    void Register(NetConnection* conn);

    /**
    * Returns the time at which the client was created.
    */
    unsigned long GetCreationTime();

    /**
    * Subscribe client to a Stream.
    * @param s - Stream to subscribe to.
    */
    void Subscribe(Stream& s);

    /**
    * Unsubscribe client from current stream.
    */
    void UnSubscribe();

    /**
    * Return subscribed stream.
    * @return Stream* - The stream client is subscribed to or NULL if client is not subscribed.
    */
    Stream* GetStream();

    /**
    *  Return set of connections for this client.
    */
    std::set<NetConnection*>& GetConnections();

    /**
    * Returns the client id.
    */
    unsigned GetID();

private:
    unsigned m_clientID;                     /**< Client identifier  */
    unsigned long m_creationTime;           /**< Creation time as time-stamp. */
    Stream* m_subscribedStream;               /**< subscribed stream */
    std::set<NetConnection*> m_connections;   /**< connections associated with client */
};


/**
* Stream listener for adding/removing clients.
*/
class StreamListener
{
public:

    /**
    * Triggered when new client is added/subscribed to stream.
    * @param s - Stream reference.
    * @param client - client added.
    */
    virtual void OnClientSubscribed(Stream& s, StreamClient& client) {}

    /**
    * Triggered when a client is removed/unsubscribed from a stream.
    * @param s - Stream reference.
    * @param client - client removed.
    */
    virtual void OnClientUnSubscribed(Stream& s, StreamClient& client) {}
};

/**
* Pipeline for streaming, encoding, transcoding and fetching multimedia streams.
*/
class Stream : public SourceObserver, public FilterObserver
{
public:
    Stream(unsigned streamID, StreamSource* source, StreamFilter* filter, StreamSink* sink);
    /* When stream gets deleted all clients will be removed automatically
     * without notification to StreamListeners from the stream. */
    virtual ~Stream();

    /**
    * Start stream.
    */
    void Play();

    /**
    * Pause stream and if Play() is called stream is started from current position.
    */
    void Pause(); // TODO maybe not needed.

    /**
    * Stop stream and if Play() is called stream is started from beginning.
    */
    void Stop();

    /**
    * Seek into the stream to the given position.
    * @param pos - position stream will be moved to.
    */
    void Seek(unsigned pos);

    /**
    * Add a client to the stream.
    * @param client - StreamClient.
    */
    void AddClient(StreamClient* client);

    /**
    * Remove client from stream.
    * @param client - client to be removed.
    */
    void RemoveClient(StreamClient* client);

    /**
    * Get the properties of the stream items/units. Can do get/set operations for various properties.
    * @return PropertyMap reference.
    */
    PropertyMap& GetProperties() { return m_propetries; }

    /**
    * Get the unique stream identifier.
    * @return unsigned - stream identifier.
    */
    unsigned GetStreamID()    { return m_streamID; }

    /**
    * Get the source/reader of the stream.
    * @return StreamSource* pointer to object.
    */
    StreamSource* GetSource() { return m_source; }

    /**
    * Get the filter/transcoder if present of the stream.
    * @return StreamFilter* pointer to object or NULL.
    */
    StreamFilter* GetFilter() { return m_filter; }

    /**
    * Get the sink/writer if present of the stream.
    * @return StreamSink* pointer to object or NULL.
    */
    StreamSink* GetSink()   { return m_sink; }

    /**
    * Get a copy of the clients list, when you are accessing the list some clients may become invalid in the time.
    * @return std::set - copy of the container holding the clients.
    */
    std::set<StreamClient*> GetClientList() { return m_clients; }

    /**
    * Add a listener/observer object to be notified on stream events.
    * @param listener - listener/observer object
    */
    void AddListener(StreamListener* listener);

    /**
    * Remove a listener/observer object.
    * @param listener - listener/observer object
    */
    void RemoveListener(StreamListener* listener);

protected:
    /* from SourceObserver */
    void OnStart(StreamSource& source);

    /* from SourceObserver */
    void OnStop(StreamSource& source);

    /* from SourceObserver */
    void OnDataReceive(StreamSource& source, Buffer* data, ErrorCode& error);

    /* from FilterObserver */
    void OnDataReady(StreamFilter* filter, Buffer* data);

private:
    unsigned      m_streamID;      /**< Stream unique identifier */
    SystemMutex    m_lockClients;   /**< lock for the clients container, provides thread safety when adding/removing clients */
    SystemMutex    m_lockListeners; /**< lock listener container, provides thread safety when adding/removing listeners */
    StreamSource*  m_source;  /**< Stream source/reader */
    StreamFilter*  m_filter;  /**< Stream encoder/decoder */
    StreamSink*    m_sink;    /**< Stream sink/writer */
    std::set<StreamClient*>      m_clients;    /**< Clients subscribed to the stream */
    std::set<StreamListener*>    m_listeners;  /**< listeners for events */
    PropertyMap                  m_propetries; /**< Properties if the stream as well as properties of the source, filter and sink */
};

#endif /* STREAM_H_ */
