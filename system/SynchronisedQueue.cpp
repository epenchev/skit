/*
 * SynchronisedQueue.cpp
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
 *  Created on: Jun 20, 2013
 *      Author: emo
 */

#include "system/SynchronisedQueue.h"
#include "system/Task.h"
#include <iostream>

template <typename T>
void SynchronisedQueue<T>::EnQueue(T data)
{
    SystemMutexLocker lock(mMutex);
    mQueue.push(data);
    mCondVar.NotifyOne();
}

template <typename T>
T SynchronisedQueue<T>::DeQueue()
{
    T result;
    SystemMutexLocker lock(mMutex);

    while (!mQueue.size())
    {
        mCondVar.Wait(lock);
    }

    result = mQueue.front();
    mQueue.pop();

    return result;
}

/*This instantiates the template with Type=int and places the instantiated code in the shared library.
Add as many explicit instantiations as you need, for all the types you need.
*/
template class SynchronisedQueue<Task*>;
/* example
template void SynchronisedQueue::EnQueue<Task*>(Task* data);
template Task* SynchronisedQueue::DeQueue<Task*>();
*/



