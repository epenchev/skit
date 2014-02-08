/*
 * IStreamPacket.h
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
 *  Created on: Jan 27, 2014
 *      Author: emo
 */

#ifndef ISTREAMPACKET_H_
#define ISTREAMPACKET_H_

#include "system/Buffer.h"

/**
* Pure virtual class interface for creating different StreamPacket types for a specific stream.
*/
class IStreamPacket
{
public:
	/**
	* Get Packet contents.
	* @return Buffer& - reference to Buffer to get the real data.
	*/
	virtual Buffer& GetData() = 0;

	/**
	* Get the timestamp of this packet.
	*/
	virtual unsigned GetTimeStamp() = 0;
};

#endif /* STREAMPACKET_H_ */
