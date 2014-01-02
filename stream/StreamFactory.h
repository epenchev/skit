/*
 * StreamFactory.h
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
 *  Created on: Dec 17, 2013
 *      Author: emo
 */

#ifndef STREAMFACTORY_H_
#define STREAMFACTORY_H_

#include "stream/Stream.h"
#include "server/PluginModule.h"

enum StreamItem { Source = 0, Filter, Sink };

/**
* StreamFacotry listener for creating/destroying streams.
*/
class StreamFactoryListener
{
public:

    /**
    * Triggered when new Stream is created.
    * @param s - reference to newly created Stream.
    */
    virtual void OnStreamCreate(Stream& s) {}

    /**
    * Triggered when a Stream is destroyed.
    * @param s - reference to the Stream to be deleted.
    */
    virtual void OnStreamDestroy(Stream& s) {}

    /**
    * Triggered when a Stream is published.
    * @param s - reference to the Stream.
    */
    virtual void OnStreamPublish(Stream& s) {}

    /**
    * Triggered when a Stream is no more published.
    * @param s - reference to the Stream.
    */
    virtual void OnStreamUnPublish(Stream& s) {}
};

/**
* Factory for creating/destroying media streams.
*/
class StreamFactory
{
public:
    /**
    * Creates a media stream and registers it to the server.
    * @param sourceClassID - ClassID for the source must be a valid object representing a real type.
    * @param filterClassID - ClassID for the filter can be NULL.
    * @param sinkClassID   - ClassID for the sink can be NULL.
    * @return Stream* - pointer to Stream object or NULL on error.
    */
    static Stream* CreateStream(const char* sourceClassID, const char* filterClassID, const char* sinkClassID);

    /**
    * Creates a media stream and gets active stream items from another stream.
    * @param s - reference to Stream object to get items from.
    * @param sourceClassID - ClassID for the source must be a valid object representing a real type.
    * @param filterClassID - ClassID for the filter can be NULL.
    * @param sinkClassID   - ClassID for the sink can be NULL.
    * @return Stream* - pointer to Stream object or NULL on error.
    */
    static Stream* CreateStream(Stream& s, const char* sourceClassID, const char* filterClassID, const char* sinkClassID);

    /**
    * Destroy a stream and removed it from the server.
    * @param s - Stream object to be destroyed.
    */
    static void DestroyStream(Stream* s);

    /**
    * Register a stream item/unit such as
    * StreamSource, StreamFilter and StreamSink into the global factory.http://www.btv.bg/
    * These items/units are used for building custom streams within the media server.
    * @param module - plug-in module holding a factory for creating objects of type StreamItem.
    * @param item - StreamItem type which this plug-in is representing.
    * @param classID - ClassID of the StreamItem, every plug-in object has a ClassID.
    */
    static void RegisterStreamItem(PluginModule* module, StreamItem item, const char* classID);

    /**
    * Publish stream with this name.
    * @param s - Reference to stream to be published.
    * @param name - name to publish this stream with.
    */
    static void PublishStream(Stream& s, const char* name);

    /**
    * Remove stream from the list with published streams and this stream is no longer visible to the public.
    * @param s - Reference to stream.
    */
    static void UnpublishStream(Stream& s);

    /**
    * Get the published stream by name.
    * @param name - Name of the published stream.
    * @return Stream* - pointer to the published stream or NULL if stream is not published.
    */
    static Stream* GetPublishedStream(const char* name);

    /**
    * Check if stream is published.
    * @param ID - Identifier of the stream.
    * @return bool - true if stream is published or false otherwise.
    */
    static bool IsPublished(unsigned ID);

    /**
    * Get the Stream published name.
    * @param ID - Identifier of the stream.
    * @return const char* - name of the stream or NULL if stream is not published.
    */
    static const char* GetStreamName(unsigned ID);

    /**
    * Add listener/observer for events from the StreamFactory
    * @param listener - listener/observer object to be notified.
    */
    static void AddListener(StreamFactoryListener* listener);

    /**
    * Remove listener/observer.
    * @param listener - listener/observer object.
    */
    static void RemoveListener(StreamFactoryListener* listener);
};

#endif /* STREAMFACTORY_H_ */
