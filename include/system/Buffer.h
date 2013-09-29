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

#include <boost/asio/buffer.hpp>
#include <boost/shared_ptr.hpp>

/**
* Buffer for raw data.
*/
class Buffer
{
public:
    virtual ~Buffer() {}

    Buffer(const Buffer& buf)
      : mBuff(boost::asio::buffer_cast<char*>(buf.mBuff), boost::asio::buffer_size(buf.mBuff)) {}

    Buffer(void* data, std::size_t size)
      : mBuff(data, size) {}

    inline Buffer& operator = (const Buffer& buf)
    {
        boost::asio::buffer_copy(this->mBuff, buf.mBuff);
        return *this;
    }

    /**
    * Create new Buffer object that is offset from the start of another.
    * @return Buffer - Buffer value.
    */
    inline friend Buffer operator + (const Buffer& buf, std::size_t start)
    {
        boost::asio::mutable_buffer newBuf(buf.mBuff + start);
        return Buffer(boost::asio::buffer_cast<void*>(newBuf), boost::asio::buffer_size(newBuf));
    }

    /**
    * Cast buffer to specific simple type (char, int ..).
    * @param buf - Buffer object to be casted.
    * @return value to the new type.
    */
    template <class T>
    inline friend T BufferCast(Buffer& buf) { return  boost::asio::buffer_cast<T>(buf.mBuff); }

    /**
    * Get the size of the buffer.
    */
    inline std::size_t GetSize() { return boost::asio::buffer_size(this->mBuff); }

private:
    boost::asio::mutable_buffers_1 mBuff;
};

typedef boost::shared_ptr<Buffer> BufferPtr;

#endif /* BUFFER_H_ */



