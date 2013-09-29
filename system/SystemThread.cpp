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
    delete mpThread;
}

void SystemThread::Start()
{
    if (!mpThread)
    {
        mpThread = new boost::thread(boost::ref(*this));
    }
}

void SystemThread::Join()
{
    if (!mpThread)
    {
        mErrCode.SetValue(EFAULT);
        throw SystemException(mErrCode);
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
            mErrCode.SetValue(err.code().value());
            throw SystemException(mErrCode);
        };
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
    	// TODO incompatible with 1.49
        //mErrCode.SetValue(err.code().value());
        throw SystemException(mErrCode);
    }
}

void SystemMutex::Unlock()
{
    mRecursiveMutex.unlock();
}

bool SystemMutex::TryLock()
{
    mErrCode.Clear();
    bool returnCode = false;

    try
    {
        returnCode = mRecursiveMutex.try_lock();
    }
    catch(const boost::thread_resource_error& err)
    {
        // TODO incompatible with 1.49
    	//mErrCode.SetValue(err.code().value());
        throw SystemException(mErrCode);
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


