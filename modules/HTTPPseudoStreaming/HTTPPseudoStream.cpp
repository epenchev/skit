#include "HTTPPseudoStream.h"
#include <utils/Logger.h>
#include <utils/ErrorCode.h>
#include <HTTP/HTTPUtils.h>
#include <server/ServerController.h>
#include <server/NetConnection.h>
#include <string>
#include <iostream>
#include <sstream>

HTTPPseudoStream::HTTPPseudoStream()
{
    IHTTPServer* server = ServerController::GetHTTPServer();
    if (server)
    {
        server->AddServerListener(this);
    }
}

void HTTPPseudoStream::OnHTTPSessionCreate(IHTTPSession& session)
{
    LOG(logINFO) << "Session created, attach listener";
    session.AddHTTPSessionListener(this);
}

void HTTPPseudoStream::OnHTTPSessionDestroy(IHTTPSession& session)
{
    LOG(logINFO) << "Session destroyed";
}

void HTTPPseudoStream::OnHTTPrequest(const IHTTPSession& session, HTTPRequest& inRequest)
{
    LOG(logINFO) << "Got HTTP request";
    LOG(logINFO) << inRequest.GetPath();
}

void HTTPPseudoStream::OnReplySend(IHTTPSession& session, ErrorCode& err)
{
    if (err)
    {
        LOG(logERROR) << err.GetErrorMessage();
    }
    LOG(logDEBUG) << "HTTP response send, disconnect";
    NetConnection& conn = dynamic_cast<NetConnection&>(session);
    LOG(logDEBUG) << "Disconnecting..";
    conn.Disconnect();

}

void HTTPPseudoStream::OnHTTPRequestReply(const IHTTPSession& session,
                                          HTTPResponse& inResponse, bool& forceReply)
{
    ErrorCode err;
    std::stringstream sSize;
    HTTPHeadersMap headers;

    LOG(logDEBUG) << "Sending response";

    const std::string data = "<html><body><h2>Hello World !!!</h2></body></html>";
    sSize << data.size();
    headers.insert(HTTPParam("Content-Length", sSize.str()));
    headers.insert(HTTPParam("Content-Type", "text/html; charset=utf-8"));
    std::string response = HTTPUtils::HTTPResponseToString(200, headers);

    inResponse.Init(response, err);
    if (err)
    {
        LOG(logERROR) << err.GetErrorMessage();
    }
    else
    {
        inResponse.SetData(data);
    }
}
