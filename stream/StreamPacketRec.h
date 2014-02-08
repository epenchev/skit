/*
 * StreamPacketRec.h
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

#ifndef STREAMPACKETREC_H_
#define STREAMPACKETREC_H_

#include "stream/IStreamPacket.h"
#include "stream/StreamPlayItem.h"

/**
* IStreamPacket type for data from recorded media streams such as files.
*/
class StreamPacketRec : public IStreamPacket
{
public:
	StreamPacketRec(Buffer& data, StreamPlayItem& playItem);
	virtual ~StreamPacketRec();

	/* from IStreamPacket */
	Buffer& GetData();

	/* from IStreamPacket */
	unsigned GetTimeStamp();

	/**
	* Get the PlayItem associated with this packet.
	* @return StreamPlayItem& - reference to PlayItem.
	*/
	const StreamPlayItem& GetPlayItem();

protected:
	Buffer* m_data;
	StreamPlayItem m_playItem;

};

#endif /* STREAMPACKETREC_H_ */
