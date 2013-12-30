/*
 * StreamSink.h
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

#ifndef STREAMSINK_H_
#define STREAMSINK_H_

#include <string>

class Buffer;
class Stream;

class StreamSink
{
public:
    /**
    * Write data to sink.
    * @param data - Buffer with data to be writen.
    * @param s    - Stream this sink is associated with.
    */
    virtual void WriteData(Buffer* data, Stream* s) = 0;
};

#endif /* STREAMSINK_H_ */
