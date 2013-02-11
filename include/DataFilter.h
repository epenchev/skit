/*
 * DataFilter.h
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
 * Created on: Jan 7, 2013
 *      Author: emo
 */

#ifndef DATAFILTER_H_
#define DATAFILTER_H_

#include "DataSource.h"
#include "DataSink.h"

namespace blitz {

class DataFilter : public DataSource, public DataSink
{
public:
        virtual ~DataFilter() {};
        DataFilter() : DataSource(), DataSink() {};

        virtual void write(DataPacket* packet)
        {
            DataSink::write(packet);
            process();
        }
protected:
        virtual void process() { DataSource::addData(this->packet_); }
};

}// blitz



#endif /* DATAFILTER_H_ */
