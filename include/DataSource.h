/*
 * DataSource.h
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
 *  Created on: Jan 6, 2013
 *      Author: emo
 */

#ifndef DATASOURCE_H_
#define DATASOURCE_H_

#include "DataSink.h"
#include "DataPacket.h"
#include <list>

namespace blitz {

/**
* Class representing data source (entry point) component in typical pipeline.
* Supports multiple sinks.
*/
class DataSource
{
public:
    DataSource() {}
    virtual ~DataSource() {}

    /**
    * inserts data into the source of the pipeline
    * @param packet: pointer to DataPacket object.
    * @throws exception in case of NULL pointer or other error
    */
    virtual void addData( blitz::DataPacket* packet );

    /**
    * adds a sink to the source, can be called several times with different sinks source supports multiple sinks.
    * @param sink: pointer to DataSink object.
    * @throws exception in case of NULL pointer or other error.
    */
    virtual void addSink(blitz::DataSink* sink);
protected:
    std::list<DataSink*> m_sink_list; /**< storage container for the sinks. */
};

} // blitz

#endif /* DATASOURCE_H_ */


