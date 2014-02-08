/*
 * StreamPacketRec.cpp
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

#include "stream/StreamPacketRec.h"

StreamPacketRec::StreamPacketRec(Buffer& data, StreamPlayItem& playItem)
{
	m_data = &data;
	m_playItem = playItem;
}

StreamPacketRec::~StreamPacketRec()
{
	delete m_data;
}

Buffer& StreamPacketRec::GetData()
{
	return *m_data;
}

unsigned StreamPacketRec::GetTimeStamp()
{
	return 0;
}

const StreamPlayItem& StreamPacketRec::GetPlayItem()
{
	return m_playItem;
}
