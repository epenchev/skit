/*
 * HTTPPseudoStreamSink.cpp
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

#include "HTTPPseudoStreamSink.h"
#include <HTTP/HTTPUtils.h>
#include <utils/Logger.h>
#include <stream/StreamFactory.h>
#include <stream/IStreamPacket.h>
#include <stream/StreamPacketRec.h>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <string>

static void SplitByteRange(const std::string& inRange, std::string& fromRange, std::string& toRange)
{
    if (!inRange.empty())
    {
        std::size_t pos = inRange.find_first_of('=');
        if (pos != std::string::npos)
        {
            std::size_t splitPos = inRange.find_first_of('-', pos);
            if (splitPos != std::string::npos)
            {
                fromRange = inRange.substr(pos + 1, (splitPos - 1) - (pos));
                if (splitPos != inRange.length())
                {
                    toRange = inRange.substr(splitPos + 1, inRange.length() - (splitPos + 1));
                }
            }
        }
    }
}

HTTPPseudoStreamSink::HTTPPseudoStreamSink()
 : m_responseBuffer(NULL), m_stream(NULL)
{}

HTTPPseudoStreamSink::~HTTPPseudoStreamSink()
{}

void HTTPPseudoStreamSink::WriteData(IStreamPacket& packet)
{
    if (m_stream)
    {
    	std::string httpResponse;
    	NetConnection* conn = NULL;

        std::set<StreamClientPtr>::iterator itActiveClient = m_clients.begin();
        for (; itActiveClient != m_clients.end(); ++itActiveClient)
        {
        	StreamPacketRec& packetRec = dynamic_cast<StreamPacketRec&>(packet);
        	const StreamPlayItem& playItem =  packetRec.GetPlayItem();

        	unsigned fromRange = (*itActiveClient)->GetProperties().GetProperty<unsigned>("fom_range", 0);
        	unsigned lengthRange = (*itActiveClient)->GetProperties().GetProperty<unsigned>("length_range", 0);

        	StreamPlayItem playItemClient(m_stream->GetStreamID(), fromRange, lengthRange);

        	if ( playItem == playItemClient )
        	{
        		LOG(logDEBUG) << "valid packet";
        		conn = *((*itActiveClient)->GetConnections().begin());
        		httpResponse = (*itActiveClient)->GetProperties().GetProperty("response");
        		break;
        	}
        }

        if (!conn)
        {
        	LOG(logERROR) << "invalid packet";
        	return;
        }
        LOG(logDEBUG) << "Valid packet for connection:" << conn->GetID();

        IOChannel* chan = NULL;
        std::set<IOChannel*>::iterator itChannel = m_channels.begin();
        for ( ; itChannel != m_channels.end(); ++itChannel )
        {
        	if ( (*itChannel)->GetConnection()->GetID() == conn->GetID() )
        	{
        		chan = *itChannel;
        		LOG(logDEBUG) << "Have open channel for conID:" << conn->GetID();
        		break;
        	}
        }

        if (!chan)
        {
        	LOG(logDEBUG) << "Opening channel for conID:" << conn->GetID();
        	chan = conn->OpenChannel(this);
        }

        Buffer& data = packet.GetData();
        std::string endOfchunk = "\r\n0\r\n\r\n";
        unsigned size = ( httpResponse.size() + data.Size() + endOfchunk.size() );

        m_responseBuffer = new Buffer(new char[size], size);
        memcpy(m_responseBuffer->Get(), httpResponse.c_str(), httpResponse.size());
        memcpy(m_responseBuffer->Get() + httpResponse.size(), data.Get(), data.Size());
        memcpy(m_responseBuffer->Get() + httpResponse.size() + data.Size(), endOfchunk.c_str(), endOfchunk.size());
        chan->Write(*m_responseBuffer);
    }
    else
    {
        LOG(logERROR) << "Sink not started";
    }
}

void HTTPPseudoStreamSink::Start(Stream& s)
{
    LOG(logINFO) << "Starting Sink";
    m_stream = &s;
    m_stream->AddListener(this);
}


void HTTPPseudoStreamSink::OnHTTPrequest(HTTPSessionPtr session, const HTTPRequest& inRequest)
{
    std::string streamName = StreamFactory::GetStreamName(m_stream->GetStreamID());
    std::string uripath = inRequest.GetPath();

    /* To be removed only for test
    std::cout << "\n\n";
    std::cout << inRequest.GetRawHeader();
    std::cout << "\n\n"; */


    if (!streamName.compare(uripath))
    {
    	LOG(logDEBUG) << "stream name: " << streamName << " matches http path" << uripath;
        std::string inRange = inRequest.GetHeader("Range");

        if (!inRange.empty())
        {
        	std::string fromRange, toRange;
            SplitByteRange(inRange, fromRange, toRange);
            LOG(logDEBUG) << "Ranges from: " << fromRange << " to: " << toRange;

            std::string fileSize = m_stream->GetProperties().GetProperty("filesize");
            if (!fileSize.empty())
            {
            	HTTPHeadersMap headers;
            	unsigned seekLength = 0;

                headers.insert(HTTPParam("Content-Type", "video/mp4"));
                if (0 == fromRange.compare("0")) // 0 from range serve only 1025 bytes from start
                {
                	seekLength = 1025;
                	std::string contentRange = "bytes 0-1024/";
                	headers.insert(HTTPParam("Content-Range", (contentRange += fileSize)));
                }
                else
                {
                	int numToRange = 0;
                	int numFromRange = atoi(fromRange.c_str());
                	if (numFromRange != 0)
                	{
                		char contentRangeHeaderValue[256] = {0};
                		unsigned numFileSize = m_stream->GetProperties().GetProperty<unsigned>("filesize", 0);

                		seekLength = ((numFileSize - numFromRange) >= m_maxPacketSizeBytes) ? (m_maxPacketSizeBytes) : (numFileSize - numFromRange);
                		numToRange = (seekLength < m_maxPacketSizeBytes) ? (numFromRange + seekLength - 1) : numFromRange + seekLength;
                		snprintf(contentRangeHeaderValue, sizeof(contentRangeHeaderValue), "bytes %d-%d/%d", numFromRange, numToRange, numFileSize);
                		headers.insert(HTTPParam("Content-Range", contentRangeHeaderValue));
                	}
                }
                char chunkLenHex[32] = {0};

                headers.insert(HTTPParam("Transfer-Encoding", "chunked"));
                headers.insert(HTTPParam("Connection", "keep-alive"));
                std::string httpResponse = HTTPUtils::HTTPResponseToString(206, headers);

                snprintf(chunkLenHex, sizeof(chunkLenHex), "%x\r\n", seekLength);
                httpResponse += chunkLenHex;

                std::set<StreamClientPtr>::iterator itActiveClient = m_clients.begin();
                for (; itActiveClient != m_clients.end(); ++itActiveClient)
                {
                	NetConnection* conn = *(*itActiveClient)->GetConnections().begin();
                	if (conn->GetID() == session->GetID())
                	{
                		LOG(logDEBUG) << conn->GetID() << " " << session->GetID();

                		(*itActiveClient)->GetProperties().SetProperty("response", httpResponse);
                		(*itActiveClient)->GetProperties().SetProperty<unsigned>("fom_range", atoi(fromRange.c_str()));
                		(*itActiveClient)->GetProperties().SetProperty<unsigned>("length_range", seekLength);
                		break;
                	}
                }

                StreamPlayItem playItem(m_stream->GetStreamID(), atoi(fromRange.c_str()), seekLength);
                m_stream->Seek(playItem);
            }
        }
        else
        {
        	LOG(logERROR) << "Missing range in request";
            // TODO create response to send this error
        	session->Disconnect();
        }
    }
    else
    {
    	LOG(logERROR) << "stream name: " << streamName << " doesn't matches http URI path: " << uripath;
        session->RemoveHTTPSessionListener(this);
        session->Disconnect();
    }
}

void HTTPPseudoStreamSink::OnClientAccepted(Stream& s, StreamClientPtr client)
{
    if (s.GetStreamID() != m_stream->GetStreamID())
    {
    	LOG(logERROR) << "No stream match";
    	return;
    }

    LOG(logINFO) << "{}";
	std::set<NetConnection*> clientConnections = client->GetConnections();
    if (!clientConnections.empty())
    {
        NetConnection* conn = *clientConnections.begin();
        HTTPSession* session = dynamic_cast<HTTPSession*>(conn);
        if (session)
        {
        	LOG(logINFO) << "{}";
        	m_clients.insert(client);
        	session->AddHTTPSessionListener(this);
            session->AddListener(this);
            // also ugly HACK
            OnHTTPrequest(session->shared_from_this(), session->GetRequest());

            // prevent session from delete OnConnectionClose from the server before we get notified. Ugly HACK
            m_httpSessions.insert(session->shared_from_this());
        }
    }
    else
    {
        LOG(logERROR) << "No connections present in client's set";
    }
}

void HTTPPseudoStreamSink::OnClientRemoved(Stream& s, StreamClientPtr client)
{
    // TODO
}


void HTTPPseudoStreamSink::OnWrite(IOChannel* chan, std::size_t bytesWritten, ErrorCode& err)
{
    if (err)
    {
        LOG(logERROR) << err.GetErrorMessage();
    }

    LOG(logDEBUG) << "writen " << bytesWritten << " bytes";

    if (m_responseBuffer)
    {
    	delete m_responseBuffer;
    	m_responseBuffer = NULL;
    }
}


void HTTPPseudoStreamSink::OnConnectionClose(NetConnection& conn)
{
    LOG(logDEBUG) << "Connection closed";
}

