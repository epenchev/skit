/*
 * FsSource.h
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
 *  Created on: Feb 25, 2013
 *      Author: emo
 */

#ifndef FSSOURCE_H_
#define FSSOURCE_H_

#include "DataSource.h"
#include "DataPacket.h"
#include <fstream>
#include <cstdio>

namespace blitz {

class FsSource : public DataSource
{
public:
    FsSource(const char* filename);
    virtual ~FsSource() {}

    void readPacket();
private:
    std::ifstream input;
    int fd;
    int bytes;
};

} // blitz

#endif /* FSSOURCE_H_ */
