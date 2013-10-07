/*
 * Buffer.cpp
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
 *  Created on: Oct 2, 2013
 *      Author: emo
 */

#include <cstring>
#include "system/Buffer.h"

Buffer::Buffer() : mbufPtr(NULL), m_size(0)
{}

Buffer::Buffer(const Buffer& buf)
{
    std::size_t size = buf.Size();

    if (size)
    {
        mbufPtr = new char[size];

        memset(mbufPtr, 0, size);
        memcpy(mbufPtr, buf.Get(), size);

        this->m_size = size;
    }
}

Buffer::Buffer(void* data, std::size_t size)
{
    if (data)
    {
        if (size)
        {
            mbufPtr = (char*)data;
            memcpy(mbufPtr, data, size);
            this->m_size = size;
        }
    }
}

Buffer::~Buffer()
{
   // delete mbufPtr;
}

Buffer& Buffer::operator = (const Buffer& buf)
{
    std::size_t size = buf.Size();

    if (size > this->m_size)
    {
        delete mbufPtr;
        mbufPtr = new char[size];
    }

    memset(mbufPtr, 0, size);
    memcpy(mbufPtr, buf.Get(), size);
    this->m_size = size;

    return *this;
}

char* Buffer::Get() const
{
    return mbufPtr;
}

std::size_t Buffer::Size() const
{
    return m_size;
}

void Buffer::Clear()
{
    if (mbufPtr)
    {
        memset(mbufPtr, 0, this->m_size);
    }
}

void Buffer::Append(const char *data, std::size_t len)
{
    if (data)
    {
        if (len)
        {
            char* oldBufPtr = mbufPtr;
            std::size_t newSize = (len + m_size);
            char* reallocPtr = new char[newSize];
            memset(reallocPtr, 0, newSize);
            memcpy(reallocPtr, mbufPtr, m_size);
            memcpy(reallocPtr + m_size, data, len);
            mbufPtr = reallocPtr;
            m_size = newSize;
            delete oldBufPtr;
        }
    }
}
