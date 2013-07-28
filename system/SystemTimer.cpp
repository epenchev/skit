/*
 * SystemTimer.cpp
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
 *  Created on: Jul 4, 2013
 *      Author: emo
 */

#include "system/SystemTimer.h"
#include "system/Task.h"
#include "system/TaskThread.h"
#include <boost/bind.hpp>

SystemTimer::SystemTimer()
 : mtimerImpl(TaskThreadPool::GetEventThread().mIOServiceLoop)
{}

void SystemTimer::ExpiresSec(unsigned int seconds)
{
    mtimerImpl.expires_from_now(boost::posix_time::seconds(seconds));
}

void SystemTimer::ExpiresUsec(unsigned int useconds)
{
    mtimerImpl.expires_from_now(boost::posix_time::microseconds(useconds));
}

void SystemTimer::AddTimerListener(TimerObserver* inListener)
{
    mEventListener = inListener;
}

void SystemTimer::Wait()
{
    mtimerImpl.async_wait(boost::bind(&SystemTimer::HandleTimer, this,
                                            boost::asio::placeholders::error));
}

void SystemTimer::Cancel()
{
    boost::system::error_code err;
    mtimerImpl.cancel(err);
    if (err)
    {
        // print Error here
        //err.message();
    }
}

void SystemTimer::HandleTimer(const boost::system::error_code& error)
{
    if (!error)
    {
        Task* timerTask = new Task();
        if (timerTask && mEventListener)
        {
            timerTask->Connect(&TimerObserver::OnExpire, mEventListener);
            TaskThreadPool::Signal(timerTask);
        }
        else
            delete timerTask;
    }
}
