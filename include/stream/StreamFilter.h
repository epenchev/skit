/*
 * StreamFilter.h
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
 *  Created on: Oct 10, 2013
 *      Author: emo
 */

#ifndef STREAMFILTER_H_
#define STREAMFILTER_H_

class Buffer;
class ErrorCode;

class FilterObserver
{
public:
	virtual ~FilterObserver() {}
    virtual void OnDataReady() = 0;
};

class StreamFilter
{
public:
	virtual ~StreamFilter() {}
    virtual void WriteData(Buffer* data) = 0;
    virtual void AddListener(FilterObserver* listener) = 0;
    virtual void RemoveListener(FilterObserver* listener) = 0;
};

#endif /* STREAMFILTER_H_ */
