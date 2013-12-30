/*
 * StreamFilter.h
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

#ifndef STREAMFILTER_H_
#define STREAMFILTER_H_

class Buffer;
class StreamFilter;

#include <string>

/**
* StreamFilter listener/observer for events.
*/
class FilterObserver
{
public:
    /**
    * Triggered when there is data ready encoded/decoded from the filter.
    * @param filter - the filter itself.
    * @param data - Buffer object with the data.
    */
    virtual void OnDataReady(StreamFilter* filter, Buffer* data) = 0;
};

/**
* Decoder/encoder for media streams implemented usually from plug-ins.
*/
class StreamFilter
{
public:
    /**
    * Give work (data) to filter.
    * @param data - Buffer with data filter will be encoding/decoding.
    */
    virtual void WriteData(Buffer* data) = 0;

    /**
    * Add a listener/observer for filter events.
    * @param listener - pointer to listener.
    */
    virtual void AddListener(FilterObserver* listener) = 0;

    /**
    * Remove a listener/observer from filter.
    * @param listener - pointer to listener to be removed.
    */
    virtual void RemoveListener(FilterObserver* listener) = 0;
};

#endif /* STREAMFILTER_H_ */
