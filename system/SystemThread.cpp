/*
 * SystemThread.cpp
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
 *  Created on: May 31, 2013
 *      Author: emo
 */

#include "system/SystemThread.h"

SystemThread::SystemThread()
: mJoined(false), mpThread(NULL)
{}

SystemThread::~SystemThread()
{
    if (mpThread)
        delete mpThread;
}

void SystemThread::Start()
{
    mErrCode.Clear();
    if (!mpThread)
        mpThread = new boost::thread(boost::ref(*this));
    else
    {
        // Error print goes here
        mErrCode.SetMessage("thread is already started");
    }
}

void SystemThread::Join()
{
    mErrCode.Clear();
    if (!mpThread)
    {
        // Error print goes here
        mErrCode.SetMessage("thread is not started");
        return;
    }

    if (!mJoined && mpThread->joinable())
    {
        try
        {
            mpThread->join();
            mJoined = true;
        }
        catch (const boost::system::system_error& err)
        {
            mJoined = false;
            // Error print goes here
            mErrCode.SetMessage(err.what());
            return;
        };
    }
    else
    {
        // Error print goes here
        mErrCode.SetMessage("thread is already joined");
        return;
    }
}

thread_id SystemThread::GetID()
{
    return  boost::this_thread::get_id();
}
void SystemThread::Sleep(unsigned int inMsec)
{
    if (inMsec)
    {
        //boost::this_thread::sleep_for(boost::chrono::milliseconds(inMsec));
        // old version compatible
        boost::this_thread::sleep(boost::posix_time::milliseconds(inMsec));
    }
}

void SystemThread::operator() ()
{
    this->Entry();
}

void SystemThread::Yield()
{
    boost::this_thread::yield();
}

void SystemMutex::Lock()
{
    mErrCode.Clear();
    try
    {
        mRecursiveMutex.lock();
    }
    catch(const boost::thread_resource_error& err)
    {
        // Error print goes here
        mErrCode.SetMessage(err.what());
        return;
    }
}

void SystemMutex::Unlock()
{
    mRecursiveMutex.unlock();
}

bool SystemMutex::TryLock()
{
    bool returnCode = false;

    try
    {
        returnCode = mRecursiveMutex.try_lock();
    }
    catch(const boost::thread_resource_error& err)
    {
        // Error print goes here
        mErrCode.SetMessage(err.what());
    }
    return returnCode;
}

SystemMutexLocker::SystemMutexLocker(SystemMutex& inMutex)
: mMutexLock(inMutex.mLockableMutex) {}


void SystemCondVariable::NotifyOne()
{
    mCondVariable.notify_one();
}

void SystemCondVariable::NotifyAll()
{
    mCondVariable.notify_all();
}

void SystemCondVariable::Wait(SystemMutexLocker& inLock)
{
    mCondVariable.wait(inLock.mMutexLock);
}


