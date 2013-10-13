/*
 * StreamSource.h
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

#ifndef STREAMSOURCE_H_
#define STREAMSOURCE_H_

class StreamSource;

class SourceObserver
{
public:
	virtual ~SourceObserver() {}
    virtual void OnStart(StreamSource* source) {}
    virtual void OnStop(StreamSource* source) {}
    virtual void OnDataReceive(StreamSource* source) {}
};

class StreamSource
{
public:
	virtual ~StreamSource() = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Seek() = 0;
    virtual bool IsSeekable() = 0;
    virtual void AddListener(SourceObserver* listener) = 0;
    virtual void RemoveListener(SourceObserver* listener) = 0;
};

#endif /* STREAMSOURCE_H_ */
