/*
 * StreamPlayItem.cpp
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
 *  Created on: Jan 13, 2014
 *      Author: emo
 */

#include "stream/StreamPlayItem.h"

StreamPlayItem::StreamPlayItem(unsigned strID, unsigned long start, unsigned long length)
 : m_streamID(strID), m_start(start), m_length(length)
{}

StreamPlayItem::~StreamPlayItem()
{}

unsigned long StreamPlayItem::GetLength() const
{
    return m_length;
}

unsigned long StreamPlayItem::GetStart() const
{
    return m_start;
}

unsigned StreamPlayItem::GetStreamID() const
{
    return m_streamID;
}

StreamPlayItem& StreamPlayItem::operator = (const StreamPlayItem& playItem)
{
    this->m_length = playItem.GetLength();
    this->m_streamID = playItem.GetStreamID();
    this->m_start = playItem.GetStart();

    return *this;
}

bool StreamPlayItem::operator == (const StreamPlayItem& playItem) const
{
    bool res = false;
    if (playItem.GetLength() == m_length &&
    	playItem.GetStart() == m_start &&
    	playItem.GetStreamID() == m_streamID)
    {
        res = true;
    }

    return res;
}
