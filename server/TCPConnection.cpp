/*
 * TCPConnection.cpp
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
 *
 *  Created on: Aug 27, 2013
 *      Author: emo
 */

#include "server/TCPConnection.h"
#include "system/Task.h"
#include "system/TaskThread.h"

TCPConnection::TCPConnection(unsigned sessionId, TCPClientSocket* inSocket)
 : mWriteBusy(false), mReadBusy(false)
{
    mConnId = sessionId;
    mSocket = inSocket;
    if (mSocket)
    {
        mSocket->AttachSocketListener(this);
    }
}

TCPConnection::~TCPConnection()
{
    mSocket->RemoveSocketListener();
    mSocket->Disconnect();

    if (!mIOChannelsMap.empty())
    {
        for (IOChannels::iterator it = mIOChannelsMap.begin(); it != mIOChannelsMap.end(); ++it)
        {
            TCPConnectionIOChannel* chan = it->second;
            if (chan)
            {
                delete chan;
            }
        }
        mIOChannelsMap.clear();
    }
    delete mSocket;
}

IOChannel* TCPConnection::OpenChannel(IOChannelObserver* inListener)
{
    if (inListener)
    {
        unsigned chanId = mIOChannelsMap.size() + 1;

        // attach channel observer at creation
        IOChannel* ioChan = new TCPConnectionIOChannel(chanId, inListener, this);

        if (ioChan)
        {
            mIOChannelsMap.insert(std::pair<unsigned, TCPConnectionIOChannel*>(chanId, (TCPConnectionIOChannel*)ioChan));
            return ioChan;
        }
    }
    return NULL;
}

void TCPConnection::CloseChannel(unsigned channelId)
{
    IOChannel* chan = (TCPConnectionIOChannel*)GetChannel(channelId);
    if (chan)
    {
        mIOChannelsMap.erase(channelId);
        delete chan;
    }
}

IOChannel* TCPConnection::GetChannel(unsigned channelId)
{
    if (channelId)
    {
        if (mIOChannelsMap.count(channelId) > 0)
        {
            TCPConnectionIOChannel* ioChan = mIOChannelsMap.at(channelId);
            return ioChan;
        }
    }
    return NULL;
}

void TCPConnection::NotifyChannel(unsigned channelId, IOAction ioOper)
{
    TCPConnectionIOChannel* ioChan = (TCPConnectionIOChannel*)GetChannel(channelId);
    if (!ioChan)
    {
        mError.SetValue(EFAULT);
        throw SystemException(mError);
    }

    switch (ioOper)
    {
        case IORead:
        case IOReadSome:
            DoRead(ioChan);
            break;
        case IOWrite:
        case IOWriteSome:
            DoWrite(ioChan);
            break;
        default:
            break;
    }
}

void TCPConnection::DoWrite(TCPConnectionIOChannel* ioChannel)
{
    mWrMutexLock.Lock();
    if (!mWriteBusy)
    {
        mWriteBusy = true;
        mWrOpenChanId = ioChannel->GetChannelId();

        try
        {
            if (IOWrite == ioChannel->mIOaction)
            {
                // will send all the data in the buffer and then signal.
                mSocket->Send(*ioChannel->mIOBuffer);
            }
            else if (IOWriteSome == ioChannel->mIOaction)
            {
                // TODO will send some data and will signal.
                //mSocket->SendSome(*ioChannel->mIOBuffer);
            }
        }
        catch (SystemException& exceptionErr)
        {
            mWriteBusy = false;
            mWrMutexLock.Unlock();
            throw;
        }
    }
    else // socket is busy
    {
        mOpenWriteChannels.push_back(ioChannel);
    }
    mWrMutexLock.Unlock();
}

void TCPConnection::DoRead(TCPConnectionIOChannel* ioChannel)
{
    mRdMutexLock.Lock();
    if (!mReadBusy)
    {
        mReadBusy = true;
        mRdOpenChanId = ioChannel->GetChannelId();
        try
        {
            if (IORead == ioChannel->mIOaction)
            {
                // will read until buffer is full and then signal.
                mSocket->Receive(*(ioChannel->mIOBuffer));
            }
            else if (IOReadSome == ioChannel->mIOaction)
            {
                // will read what data is present from network.
                mSocket->ReceiveSome(*(ioChannel->mIOBuffer));
            }
        }
        catch (SystemException& exceptionErr)
        {
            mReadBusy = false;
            mRdMutexLock.Unlock();
            throw;
        }
    }
    else  // socket is busy
    {
        mOpenReadChannels.push_back(ioChannel);
    }
    mRdMutexLock.Unlock();
}

void TCPConnection::Close()
{
   // if (this->IsConnected())
    {
        mSocket->Disconnect();
        if (!mIOChannelsMap.empty())
        {
            for (IOChannels::iterator it = mIOChannelsMap.begin(); it != mIOChannelsMap.end(); ++it)
            {
                TCPConnectionIOChannel* ioChann = it->second;
                if (ioChann)
                {
                    Task* closeTask = new Task();
                    closeTask->Connect(&IOChannelObserver::OnConnectionClose, ioChann->mListener, ioChann);
                    TaskThreadPool::Signal(closeTask);
                }
            }
        }
    }
}

bool TCPConnection::IsConnected()
{
    if (mSocket)
    {
        //return mSocket->IsOpen();
    }
    return false;
}

std::string TCPConnection::GetRemoteAddress()
{
/*
    TODO in socket
    if (mSocket->IsOpen())
    {
        return mSocket->GetRemotePeerIP();
    }*/
    return "";

}

unsigned TCPConnection::GetRemotePort()
{
    /*
     * TODO in socket
    if (mSocket->IsOpen())
    {
        return mSocket->GetRemotePeerPort();
    }
    */
    return 0;

}


void TCPConnection::OnSend(ClientSocket* inSocket)
{
    if (inSocket)
    {
        mWrMutexLock.Lock();

        ErrorCode err = mSocket->GetLastError();
        TCPConnectionIOChannel* ioChannel = (TCPConnectionIOChannel*)GetChannel(mWrOpenChanId);
        if (!ioChannel)
        {
            // TODO log here
            mWrMutexLock.Unlock();
            return;
        }

        mWrOpenChanId = 0;
        if (err)
        {
            ioChannel->mError = err;
        }

        Task* onWriteTask = new Task();
        onWriteTask->Connect(&IOChannelObserver::OnWrite, ioChannel->mListener,
                                          ioChannel, mSocket->GetBytesTransfered());
        TaskThreadPool::Signal(onWriteTask);

        // check if we have open channels waiting
        if (!mOpenWriteChannels.empty())
        {
            ioChannel = mOpenWriteChannels.front();
            mOpenWriteChannels.pop_front();
            mWrOpenChanId = ioChannel->GetChannelId();

            if (IOWrite == ioChannel->mIOaction)
            {
                mSocket->Send(*ioChannel->mIOBuffer);
            }
            else if (IOWriteSome == ioChannel->mIOaction)
            {
                // TODO with socket
                //mSocket->SendSome(*ioChannel->mIOBuffer);
            }
        }
        else
        {
            mWriteBusy = false;
        }

        mWrMutexLock.Unlock();
    }
}

void TCPConnection::OnReceive(ClientSocket* inSocket)
{
    if (inSocket)
    {
        mRdMutexLock.Lock();

        ErrorCode err = mSocket->GetLastError();
        TCPConnectionIOChannel* ioChannel = (TCPConnectionIOChannel*)GetChannel(mRdOpenChanId);

        if (!ioChannel)
        {
            // TODO log
            mRdMutexLock.Unlock();
            return;
        }

        mRdOpenChanId = 0;
        if (err)
        {
            ioChannel->mError = err;
        }

        Task* onReadTask = new Task();
        onReadTask->Connect(&IOChannelObserver::OnRead, ioChannel->mListener,
                                  ioChannel, mSocket->GetBytesTransfered());
        TaskThreadPool::Signal(onReadTask);

        // check if we have open channels waiting
        if (!mOpenReadChannels.empty())
        {
            ioChannel = mOpenReadChannels.front();
            mOpenReadChannels.pop_front();

            mRdOpenChanId = ioChannel->GetChannelId();
            if (IORead == ioChannel->mIOaction)
            {
                mSocket->Receive(*ioChannel->mIOBuffer);
            }
            else if (IOReadSome == ioChannel->mIOaction)
            {
                mSocket->ReceiveSome(*ioChannel->mIOBuffer);
            }
        }
        else
        {
            mReadBusy = false;
        }
        mRdMutexLock.Unlock();
    }
}

void TCPConnection::OnConnect(ClientSocket* inSocket)
{
    // No need to implement this, this connection is to be created from server.
    return;
}

TCPConnectionIOChannel::TCPConnectionIOChannel(const TCPConnectionIOChannel& chan)
{
    std::cout << "TCPConnectionIOChannel(const TCPConnectionIOChannel& chan) \n";
    this->mConnection = chan.mConnection;
    this->mChannelId = chan.mChannelId;
    this->mListener = chan.mListener;
}

TCPConnectionIOChannel::TCPConnectionIOChannel(unsigned channId, IOChannelObserver* listener, TCPConnection* conn)
{
    this->mConnection = conn;
    this->mChannelId = channId;
    this->mListener = listener;
}

void TCPConnectionIOChannel::Emit(Buffer* data, IOAction ioOper)
{
    if (data)
    {
        mIOaction = ioOper;
        this->mIOBuffer = data;
        try
        {
            mConnection->NotifyChannel(mChannelId, mIOaction);
        }
        catch(SystemException& ex)
        {
            // TODO Log error here
            throw;
        }
    }
    else
    {
        mError.SetValue(EINVAL);
        throw SystemException(mError);
    }
}

unsigned TCPConnectionIOChannel::GetSessionId()
{
    return mConnection->GetSessionId();
}

unsigned TCPConnectionIOChannel::GetChannelId()
{
    return mChannelId;
}



