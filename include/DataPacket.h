/*
 * DataPacket.h
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
 *  Created on: Jan 2, 2013
 *      Author: emo
 */

#ifndef DATAPACKET_H_
#define DATAPACKET_H_

#include <cstddef>
#include <iostream>

namespace blitz {

/**
* Class representing data container passed between pipeline components.
*/
class DataPacket
{
public:
    DataPacket();
    ~DataPacket() {}

    int size(void) const;
    void size(int bytes);
    void* data(void);
    void  clear(void);

    static const int max_size = 1024;
protected:
    char m_data[max_size];
    std::size_t m_used_bytes;
};

} // blitz

#endif /* DATAPACKET_H_ */
