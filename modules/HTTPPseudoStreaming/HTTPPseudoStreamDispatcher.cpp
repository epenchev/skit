#include "HTTPPseudoStreamDispatcher.h"
#include <utils/Logger.h>
#include <utils/ErrorCode.h>
#include <HTTP/HTTPUtils.h>
#include <server/ServerController.h>
#include <server/NetConnection.h>
#include <stream/StreamFactory.h>

HTTPPseudoStreamDispatcher::HTTPPseudoStreamDispatcher()
{
    HTTPServer* server = ServerController::GetHTTPServer();
    if (server)
    {
        server->AddServerListener(this);
    }
}

void HTTPPseudoStreamDispatcher::OnHTTPSessionAccept(HTTPSessionPtr session)
{
    session->AddHTTPSessionListener(this);
}

void HTTPPseudoStreamDispatcher::OnHTTPrequest(HTTPSessionPtr session, const HTTPRequest& inRequest)
{
    session->RemoveHTTPSessionListener(this);
    std::string streamName = inRequest.GetPath();
    if (!streamName.empty())
    {
    	LOG(logINFO) << "Requesting stream:" << streamName;

        Stream* s = StreamFactory::GetPublishedStream(streamName.c_str());
        if (s)
        {
        	LOG(logINFO) << "Add client to stream";
        	StreamClientPtr client(new StreamClient());
        	client->Register(dynamic_cast<NetConnection*>(session.get()));
        	s->AddClient(client);
        	client->GetProperties().SetProperty("stream_name", streamName);
        }
        else
        {
        	LOG(logERROR) << "No stream published with this name:" << streamName;
        	// TODO send response from here
        }
    }
    else
    {
    	LOG(logERROR) << "No stream name present in request";
    	// TODO send response from here
    }
}


