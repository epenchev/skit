/*
 * DataSource.cpp
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
 *  Created on: Jan 13, 2013
 *      Author: emo
 */

#include "DataSource.h"
#include <stdexcept>
#include <iostream>
#include "logging/logging.h"

using namespace ::logging;

namespace blitz {

void DataSource::addData(blitz::DataPacket* packet)
{
    if (!packet)
        throw std::invalid_argument("NULL pointer for DataPacket from DataSource::addData()");

    if (!m_sink_list.empty())
    {
        for (std::list<DataSink*>::iterator it = m_sink_list.begin(); it != m_sink_list.end(); ++it)
        {
            DataSink* sink = *it;
            sink->write(packet);
        }
    }
    else
        throw std::logic_error("DataSource::addData() no sink in list");
}

void DataSource::addSink(blitz::DataSink* sink)
{
    if (!sink)
        throw std::invalid_argument("NULL pointer for DataSink from DataSource::addSink()");

    try
    {
        m_sink_list.push_back(sink);
    }
    catch (std::bad_alloc& ex)
    {
        log::emit< Error>() << "DataSource::addSink() exception std::bad_alloc from list" << log::endl;
        throw;
    };
}

} // blitz
