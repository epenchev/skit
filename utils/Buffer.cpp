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
 *  Created on: Feb 12, 2014
 *      Author: emo
 */

#include "utils/Buffer.h"
#include <cstring>

Buffer::Buffer()
: m_copyBuff(NULL), m_copyDirty(false), m_copySize(0)
{}

Buffer::Buffer(unsigned bytes)
 : m_copyBuff(NULL), m_copyDirty(false), m_copySize(0)
{
	resize(bytes);
	m_copySize = size();

	if (m_copySize)
	{
		m_copyBuff = new (char[m_copySize]);
		memset(m_copyBuff, 0, m_copySize);
	}
}

Buffer::Buffer(const Buffer& buf)
: m_copyBuff(NULL), m_copyDirty(false), m_copySize(0)
{
	assign(buf.data(), buf.size());
}

Buffer::~Buffer()
{
	delete m_copyBuff;
}

Buffer& Buffer::operator = (const Buffer& buf)
{
	assign(buf.data(), buf.size());
	return *this;
}

char* Buffer::get()
{
	char* copyPtr = NULL;
	size_t dataSize = size();

	if (dataSize && !m_copyDirty)
	{
		if (m_copySize != dataSize)
		{
			delete m_copyBuff;
			m_copyBuff = new (char[dataSize]);
			m_copySize = dataSize;
		}

		/* '\0' character is not appended here, it's raw buffer */
		copy(m_copyBuff, m_copySize);
		m_copyDirty = true;

		copyPtr = m_copyBuff;
	}

	return copyPtr;
}

void Buffer::commit(unsigned int bytes = 0)
{
	if (m_copyDirty)
	{
		if (bytes)
		{
			assign(m_copyBuff, bytes);
		}
		else
		{
			size_t size_bytes = size();
			assign(m_copyBuff, size_bytes);
		}

		memset(m_copyBuff, 0, m_copySize);
		m_copyDirty = false;
	}
}

