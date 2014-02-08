/*
 * HTTPPseudoStreamSink.h
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
 *  Created on: Jan 14, 2014
 *      Author: emo
 */

#ifndef HTTPPSEUDOSTREAMSINK_H_
#define HTTPPSEUDOSTREAMSINK_H_

#include <server/IOChannel.h>
#include <server/NetConnection.h>
#include <server/PluginModule.h>
#include <stream/Stream.h>
#include <stream/StreamClient.h>
#include <stream/StreamSink.h>
#include <stream/StreamPlayItem.h>
#include <utils/ErrorCode.h>
#include <HTTP/HTTPRequest.h>
#include <HTTP/HTTPResponse.h>
#include <server/HTTPServer.h>
#include <system/Buffer.h>
#include <set>

class HTTPPseudoStreamSink
: public PluginObject, public StreamSink,
  public IOChannelListener, public NetConnectionListener,
  public StreamListener, public HTTPSessionListener
{
public:
    HTTPPseudoStreamSink();
    virtual ~HTTPPseudoStreamSink();

    /* from StreamSink */
    void WriteData(IStreamPacket& packet);

    /* from StreamSink */
    void Start(Stream& s);

    /* from IOChannel */
    void OnWrite(IOChannel* chan, std::size_t bytesWritten, ErrorCode& err);

    /* from NetConnection */
    void OnConnectionClose(NetConnection& conn);

    /* from PluginObject */
    const char* GetClassID() { return "HTTPPseudoStreamSink"; }

    /* from StreamListener */
    void OnClientAccepted(Stream& s, StreamClientPtr client);

    /* from StreamListener */
    void OnClientRemoved(Stream& s, StreamClientPtr client);

    /* from HTTPSessionListener */
    void OnHTTPrequest(HTTPSessionPtr session, const HTTPRequest& inRequest);

private:
    Buffer* m_responseBuffer;
    Stream* m_stream;
    std::set<StreamClientPtr> m_clients;
    std::set<IOChannel*> m_channels;
    std::set<HTTPSessionPtr> m_httpSessions;

    static const unsigned int m_maxPacketSizeBytes = 16000;
};

#endif /* HTTPPSEUDOSTREAMSINK_H_ */
