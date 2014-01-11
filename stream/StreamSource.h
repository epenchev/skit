/*
 * StreamSource.h
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

#ifndef STREAMSOURCE_H_
#define STREAMSOURCE_H_

#include <string>

class StreamSource;
class Buffer;
class ErrorCode;
class Stream;

/* #define BUILD_MOD */
#ifdef BUILD_MOD
#define EXPORT_PUB __attribute__ ((visibility ("default")))
#else
#define EXPORT_PUB
#endif

/**
* StreamSource listener/observer for events.
*/
class SourceObserver
{
public:

    /**
    * Triggered when the source is started.
    * @param source - the source itself
    */
    virtual void OnStart(StreamSource& source) {}

    /**
    * Triggered when the source is stopped.
    * @param source - the source itself
    */
    virtual void OnStop(StreamSource& source) {}

    /**
    * Triggered when there is data from the source.
    * @param source - the source itself.
    * @param data - pointer to Buffer with data from source.
    * @param error - error code if present when reading the source.
    */
    virtual void OnDataReceive(StreamSource& source, Buffer* data, ErrorCode& error) {}
};

/**
* Source/reader for media streams implemented usually from plug-ins.
*/
class EXPORT_PUB StreamSource
{
public:

    /**
    * Start the source.
    */
    virtual void Start(Stream& s) = 0;

    /**
    * Stop the source.
    */
    virtual void Stop() = 0;

    /**
    * Seek into a given position into the stream and read length bytes if provided,
    * otherwise will read until the end of the stream.
    * @param position - position to seek to.
    * @param length - length bytes to read.
    */
    virtual void Seek(unsigned position, unsigned length = 0) = 0;

    /**
    * Checks if source is seek-able.
    * @return true if seek-able false otherwise.
    */
    virtual bool IsSeekable() = 0;

    /**
    * Add a listener/observer for source events.
    * @param listener - pointer to listener.
    */
    virtual void AddListener(SourceObserver* listener) = 0;

    /**
    * Remove a listener/observer from source.
    * @param listener - pointer to listener to be removed.
    */
    virtual void RemoveListener(SourceObserver* listener) = 0;
};

#endif /* STREAMSOURCE_H_ */
