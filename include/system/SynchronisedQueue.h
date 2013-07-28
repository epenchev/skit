/*
 * SynchronisedQueue.h
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
 *  Created on: Jun 19, 2013
 *      Author: emo
 */

#ifndef SYNCHRONISEDQUEUE_H_
#define SYNCHRONISEDQUEUE_H_

#include <queue>
#include <boost/noncopyable.hpp>
#include "SystemThread.h"

/**
* Queue class that has thread synchronization.
*/
template <typename T>
class SynchronisedQueue
{
public:
    SynchronisedQueue() {}
    virtual ~SynchronisedQueue() {}

    /**
    * Add data to the queue and notify others.
    * @param data - Reference to data to be added in the queue.
    */
    void EnQueue(T data);

    /**
    * Get data from the queue. Wait for data if not available
    * @note - Will block until EnQueue() is invoked from another thread
    *         and data is added.
    */
    T DeQueue();

    /**
    * @return element count.
    */
    unsigned GetLength() { return mQueue.size(); }

private:
    std::queue<T> mQueue;         /**< STL queue */
    SystemMutex mMutex;           /**< The mutex to synchronise on */
    SystemCondVariable mCondVar;  /**< The condition to wait for */
};

#endif /* SYNCHRONISEDQUEUE_H_ */
