/*
 * DataSink.h
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
 * Created on: Jan 6, 2013
 *      Author: emo
 */

#ifndef DATASINK_H_
#define DATASINK_H_

#include "DataPacket.h"

namespace blitz {

/**
* Class representing data sink component in typical pipeline.
*/
class DataSink
{
public:
    DataSink() { m_packet = (blitz::DataPacket*) 0; }
    virtual ~DataSink() { }
    virtual void write(blitz::DataPacket* p) { m_packet = p; }
    virtual blitz::DataPacket* getPacket() { return m_packet; }

protected:
    blitz::DataPacket* m_packet;
};

} // blitz

#endif /* DATASINK_H_ */
