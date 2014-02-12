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
 *  Created on: Feb 11, 2014
 *      Author: emo
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <string>

/**
* Extending standard string class to be used as a buffer for raw data.
* All string functions are acceptable as is,
* only the assignment operator is an exception and has its own implementation.
*/
class Buffer : public std::string
{
public:
	/* Constructors */
	Buffer();
	Buffer(unsigned bytes);
	Buffer(const Buffer& buf);

	virtual ~Buffer();

	/**
	* Default string operator(=) will not work, it's replaced with this one.
	* All other operators works like as performed on a string object.
	*/
	Buffer& operator = (const Buffer& buf);

	/**
	* Get a writable copy of the buffer
	* @return char* - valid pointer or NULL in case buffer is not allocated or there is commit pending.
	*/
	char* get();

	/* Get the state of the copy buffer.
	 * If true commit() must be called before any other operation or data may be lost. */
	bool dirty();

	/**
	* Commit pending data in copy buffer.
	* @param bytes - how many bytes to commit, if omitted the full length of the buffer will be written/commit.
	*/
	void commit(unsigned int bytes);

private:
	char* m_copyBuff;   /**< writable copy of string internal buffer */
	bool  m_copyDirty;  /**< if data is pending in copyBuff */
	size_t m_copySize;  /**< size of copyBuff */
};

#endif /* BUFFER_H_ */
