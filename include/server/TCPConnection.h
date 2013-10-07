/*
 * TCPConnection.h
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
 *  Created on: Aug 27, 2013
 *      Author: emo
 */

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include "NetConnection.h"
#include "ErrorCode.h"
#include "system/TCPClientSocket.h"
#include "system/SystemThread.h"
#include <list>
#include <set>
#include <map>

class TCPConnection;

class TCPConnectionIOChannel : public IOChannel
{
public:
    TCPConnectionIOChannel(const TCPConnectionIOChannel& chan);
    TCPConnectionIOChannel(unsigned channId, IOChannelObserver* listener, TCPConnection* conn);
    virtual ~TCPConnectionIOChannel() {}

    void Emit(Buffer* data, IOAction ioOper, ErrorCode& outError);

    unsigned GetConnId();

    unsigned GetChannelId();

private:
    friend class TCPConnection;
    Buffer*  mIOBuffer;
    unsigned mbytesTransfered;
    IOAction mIOaction;
    ErrorCode mError;
    TCPConnection* mConnection;
    IOChannelObserver* mListener;
    unsigned mChannelId;
};

typedef std::map<unsigned, TCPConnectionIOChannel*> IOChannels;
typedef std::list<TCPConnectionIOChannel*> ListIOChannels;

/**
* Class for TCP connection objects used in different TCP servers.
* This is a object created from a server and bound to it.
*/
class TCPConnection : public NetConnection, public TCPClientSocketObserver
{
public:
    TCPConnection(unsigned sessionId, TCPClientSocket* inSocket);
    virtual ~TCPConnection();

    /* From NetConnection */
    bool IsConnected();

    /* From NetConnection */
    unsigned GetConnId() { return mConnId; }

    /* From NetConnection */
    std::string GetRemoteAddress();

    /* From NetConnection */
    unsigned GetRemotePort();

    /* From NetConnection */
    void Close();

    /* From NetConnection */
    IOChannel* OpenChannel(IOChannelObserver* inListener);

    /* From NetConnection */
    void CloseChannel(unsigned channelId);

    /* From NetConnection */
    void NotifyChannel(unsigned channelId, IOAction ioOper, ErrorCode& err);

    /* From NetConnection */
    IOChannel* GetChannel(unsigned channelId);

protected:

    /* From ClientSocketObserver */
    void OnSend(TCPClientSocket* inSocket, unsigned sendBytes, ErrorCode* inError);

    /* From ClientSocketObserver */
    void OnReceive(TCPClientSocket* inSocket, unsigned receivedBytes, ErrorCode* inError);

    /* From ClientSocketObserver */
    void OnConnect(TCPClientSocket* inSocket, ErrorCode* inError);

    bool mWriteBusy;
    bool mReadBusy;

    unsigned mRdOpenChanId;
    unsigned mWrOpenChanId;


    unsigned long mConnId;        /**< unique connection ID  */
    TCPClientSocket* mSocket;     /**< socket bound with this connection */
    ErrorCode  mError;            /**< error code of last operation */

    SystemMutex mWrMutexLock;
    SystemMutex mRdMutexLock;
    ListIOChannels mOpenWriteChannels;
    ListIOChannels mOpenReadChannels;
    IOChannels mIOChannelsMap;

private:
    enum IOEvent { OnReadEvent = 0, OnWriteEvent, OnCloseEvent };

    void DoWrite(TCPConnectionIOChannel* ioChannel);

    void DoRead(TCPConnectionIOChannel* ioChannel);

    void NotifyChannelListener(TCPConnectionIOChannel* ioChannel, TCPConnection::IOEvent event);

};


#endif /* TCPCONNECTION_H_ */
