/*
 * StreamPlayItem.h
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

#ifndef STREAMPLAYITEM_H_
#define STREAMPLAYITEM_H_

#include <string>

/**
* Single unit to be played with recorded streams when seek operations are performed.
*/
class StreamPlayItem
{
public:
    StreamPlayItem(unsigned strID = 0, unsigned long start = 0, unsigned long length = 0);
    virtual ~StreamPlayItem();

    /**
    * Returns size of the item.
    */
    unsigned long GetLength() const;

    /**
    * Returns start offset mark.
    */
    unsigned long GetStart() const;

    /**
    * Returns item name which is the name of the stream.
    */
    unsigned GetStreamID() const;

    StreamPlayItem& operator = (const StreamPlayItem& playItem);
    bool operator == (const StreamPlayItem& playItem) const;

private:
    unsigned m_streamID;    /**< ID of the stream this item is part of */
    unsigned long m_start;  /**< offset mark from the start (can be bytes, frame count or time duration ) */
    unsigned long m_length; /**< length (can be bytes, frame count or time duration) for this item */
};

#endif /* STREAMPLAYITEM_H_ */
