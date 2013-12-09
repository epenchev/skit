/*
 * Buffer.h
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
 *  Created on: Jun 24, 2013
 *      Author: emo
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <cstddef>

/**
* Buffer for raw data.
*/
class Buffer
{
public:

    /**
     * Constructors.
    */
    Buffer();
    Buffer(const Buffer& buf);
    Buffer(void* data, std::size_t size);

    virtual ~Buffer();

    Buffer& operator = (const Buffer& buf);

    /**
    * Get the full size of the buffer.
    */
    std::size_t Size() const;

    /**
    * Get internal pointer to memory in the Buffer.
    */
    char* Get() const;

    /**
    * Clear the contents of the buffer.
    */
    void Clear();

    /**
    * Append memory to the end of the buffer.
    * @param memBytes - pointer to memory to append.
    * @param len - size of memory to append.
    */
    void Append(const char *data, std::size_t len);

    /**
    * Cast buffer to specific simple type (char, int ..).
    */
    template <class T>
    T BufferCast() { return static_cast<T>(mbufPtr); }

private:
    char* mbufPtr;
    std::size_t m_size;
};

// TODO add create header
#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<Buffer> BufferPtr;

#endif /* BUFFER_H_ */



