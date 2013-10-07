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
        ErrorCode err;
        mSocket->SetListener(this, err);
    }
}

TCPConnection::~TCPConnection()
{
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

void TCPConnection::NotifyChannel(unsigned channelId, IOAction ioOper, ErrorCode& err)
{
    TCPConnectionIOChannel* channel = (TCPConnectionIOChannel*)GetChannel(channelId);
    if (!channel)
    {
        err.SetValue(EFAULT);
        return;
    }

    if (IOWrite == ioOper)
    {
        DoWrite(channel);
    }
    else // IORead, IOReadSome
    {
        DoRead(channel);
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
            mSocket->Send(*ioChannel->mIOBuffer);
        }
        catch (SystemException& exceptionErr)
        {
            mWriteBusy = false;
            mWrMutexLock.Unlock();

            ioChannel->mError = exceptionErr.Code();
            NotifyChannelListener(ioChannel, TCPConnection::OnWriteEvent);
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
            ioChannel->mError = exceptionErr.Code();
            NotifyChannelListener(ioChannel, TCPConnection::OnReadEvent);
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
    if (this->IsConnected())
    {
        mSocket->Disconnect();
        if (!mIOChannelsMap.empty())
        {
            for (IOChannels::iterator it = mIOChannelsMap.begin(); it != mIOChannelsMap.end(); ++it)
            {
                TCPConnectionIOChannel* ioChannel = it->second;
                if (ioChannel)
                {
                     NotifyChannelListener(ioChannel, TCPConnection::OnCloseEvent);
                }
            }
        }
    }
}

bool TCPConnection::IsConnected()
{
    if (mSocket)
    {
        return mSocket->IsOpen();
    }
    return false;
}

std::string TCPConnection::GetRemoteAddress()
{
    std::string address;
    if (mSocket->IsOpen())
    {
        ErrorCode outErr;
        address = mSocket->GetRemotePeerIP(outErr);
    }
    return address;

}

unsigned TCPConnection::GetRemotePort()
{
    unsigned port;
    if (mSocket->IsOpen())
    {
        ErrorCode outErr;
        port = mSocket->GetRemotePeerPort(outErr);
    }
    return port;
}

void TCPConnection::NotifyChannelListener(TCPConnectionIOChannel* ioChannel, TCPConnection::IOEvent event)
{
    if (ioChannel)
    {
        Task* runTask = new Task();

        if (TCPConnection::OnWriteEvent == event)
        {
            runTask->Connect(&IOChannelObserver::OnWrite, ioChannel->mListener,
                             ioChannel, ioChannel->mbytesTransfered, &ioChannel->mError);
        }
        else if (TCPConnection::OnReadEvent == event)
        {
            runTask->Connect(&IOChannelObserver::OnRead, ioChannel->mListener,
                             ioChannel, ioChannel->mbytesTransfered, &ioChannel->mError);
        }
        else if (TCPConnection::OnCloseEvent == event)
        {
            runTask->Connect(&IOChannelObserver::OnConnectionClose, ioChannel->mListener, ioChannel);
        }
        TaskThreadPool::Signal(runTask);
    }
}

void TCPConnection::OnSend(TCPClientSocket* inSocket, unsigned sendBytes, ErrorCode* inError)
{
    if (inSocket)
    {
        mWrMutexLock.Lock();
        TCPConnectionIOChannel* ioChannel = (TCPConnectionIOChannel*)GetChannel(mWrOpenChanId);
        if (!ioChannel)
        {
            // TODO log here
            mWrMutexLock.Unlock();
            return;
        }

        mWrOpenChanId = 0;
        ioChannel->mError = *inError;
        ioChannel->mbytesTransfered = sendBytes;
        NotifyChannelListener(ioChannel, TCPConnection::OnWriteEvent);

        // check if we have open channels waiting to send
        bool sendError = true;
        while (sendError)
        {
            if (!mOpenWriteChannels.empty())
            {
                ioChannel = mOpenWriteChannels.front();
                mOpenWriteChannels.pop_front();
                mWrOpenChanId = ioChannel->GetChannelId();
                try
                {
                    mSocket->Send(*ioChannel->mIOBuffer);
                    sendError = false;
                }
                catch (SystemException& exceptionErr)
                {
                    NotifyChannelListener(ioChannel, TCPConnection::OnWriteEvent);
                    sendError = true;
                }
            }
            else
            {
                sendError = false;
                mWriteBusy = false;
            }
        }
        mWrMutexLock.Unlock();
    }
}

void TCPConnection::OnReceive(TCPClientSocket* inSocket, unsigned receivedBytes, ErrorCode* inError)
{
    if (inSocket)
    {
        mRdMutexLock.Lock();
        TCPConnectionIOChannel* ioChannel = (TCPConnectionIOChannel*)GetChannel(mRdOpenChanId);

        if (!ioChannel)
        {
            // TODO log
            mRdMutexLock.Unlock();
            return;
        }

        mRdOpenChanId = 0;
        ioChannel->mError = *inError;
        ioChannel->mbytesTransfered = receivedBytes;
        NotifyChannelListener(ioChannel, TCPConnection::OnReadEvent);

        // check if we have open channels waiting for receive
        bool recvError = true;
        while (recvError)
        {
            if (!mOpenReadChannels.empty())
            {
                ioChannel = mOpenReadChannels.front();
                mOpenReadChannels.pop_front();

                mRdOpenChanId = ioChannel->GetChannelId();
                try
                {
                    if (IORead == ioChannel->mIOaction)
                    {
                        mSocket->Receive(*ioChannel->mIOBuffer);
                        recvError = false;
                    }
                    else if (IOReadSome == ioChannel->mIOaction)
                    {
                        mSocket->ReceiveSome(*ioChannel->mIOBuffer);
                        recvError = false;
                    }
                }
                catch (SystemException& exceptionErr)
                {
                    NotifyChannelListener(ioChannel, TCPConnection::OnReadEvent);
                    recvError = true;
                }
            }
            else
            {
                recvError = false;
                mReadBusy = false;
            }
        }
        mRdMutexLock.Unlock();
    }
}

void TCPConnection::OnConnect(TCPClientSocket* inSocket, ErrorCode* inError)
{
    // No need to implement this, this connection is to be created from server.
    return;
}

TCPConnectionIOChannel::TCPConnectionIOChannel(const TCPConnectionIOChannel& chan)
 : mIOBuffer(NULL), mbytesTransfered(0)
{
    this->mConnection = chan.mConnection;
    this->mChannelId = chan.mChannelId;
    this->mListener = chan.mListener;
}

TCPConnectionIOChannel::TCPConnectionIOChannel(unsigned channId, IOChannelObserver* listener, TCPConnection* conn)
 : mIOBuffer(NULL), mbytesTransfered(0), mConnection(conn), mListener(listener), mChannelId(channId)
{}

void TCPConnectionIOChannel::Emit(Buffer* data, IOAction ioOper, ErrorCode& outError)
{
    if (data)
    {
        mIOaction = ioOper;
        this->mIOBuffer = data;
        mConnection->NotifyChannel(mChannelId, mIOaction, outError);

        if (outError)
        {
            // TODO Log error here
            return;
        }
    }
    else
    {
        outError.SetValue(EINVAL);
        // TODO Log error here
    }
}

unsigned TCPConnectionIOChannel::GetConnId()
{
    return mConnection->GetConnId();
}

unsigned TCPConnectionIOChannel::GetChannelId()
{
    return mChannelId;
}



