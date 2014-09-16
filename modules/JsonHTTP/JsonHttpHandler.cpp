#include "HTTP.h"
#include "JsonHttpHandler.h"
#include "ServerController.h"

#undef LOG_DISABLE
#include "Logger.h"

#include <sstream>
#include <cstdlib>
extern "C" {
#include <ctype.h>
}

ListenerRegistrator<JSON_RequestHandler, HttpServer> regJsonListener;

static void inline TrimString(string& inString)
{
    string::iterator theIter = inString.begin();
    // skipping leading spaces
    while (isspace(*theIter))
        ++theIter;
    if (theIter != inString.end() && theIter != inString.begin())
            inString.erase(inString.begin(), theIter);

    theIter = inString.end();
    // skipping trailing spaces
    while ( isspace(*(theIter-1)) )
        --theIter;
    if (theIter != inString.end())
        inString.erase(theIter, inString.end());
}

void JSON_RequestHandler::OnSendHandler( HttpSessionPtr inSession,
                                         const SysError& error,
                                         size_t bytesWriten )
{
    if (error)
    {
        LOG(logERROR) << "Error sending data :" << error.message();
        inSession->GetSocket()->Close();
        //fPlayer->Pause();
    }
    else
    {
        delete fOutBuf; // free memory
        //LOG(logDEBUG) << "Send " << bytesWriten <<  " bytes";
#if 1
        if (fPlayer)
            fOutBuf = fPlayer->Get();
        if (fOutBuf)
        {
            Buffer* b = new Buffer(fOutBuf->Get<char*>(), fOutBuf->Size());

            TcpSocket* theSocket = inSession->GetSocket();
            theSocket->Send(CreateBufferSequence(*b),
                            BIND_HANDLER(&JSON_RequestHandler::OnSendHandler,
                            inSession) );
        }
        else
        {
            LOG(logWARNING) << "Buffer is empty";
            inSession->GetSocket()->Close();
        }
#endif
    }
}

bool JSON_RequestHandler::OnHttpSession( HttpSessionPtr inSession, Skit::HTTP::Request& request )
{
    string theStreamName = request.GetURL();
    TrimString(theStreamName);

    LOG(logDEBUG) << "Got session, requesting: " << '[' << theStreamName << ']';

    ServerController& theController = ServerController::Instance();
    fStream = theController.GetStream(theStreamName);
    if (!fStream)
        return false;
    inSession->SetListener(this);
    fPlayer = new Player(inSession->GetSocket()->GetThreadID());
    fPlayer->Play(fStream);
    HandleRequest(inSession, request);
    return true;
}

void JSON_RequestHandler::OnHttpRequest( HttpSessionPtr session,
                                         Skit::HTTP::Request& request,
                                         const SysError& error )
{
	LOG(logDEBUG) << "Got request here";
    LOG(logDEBUG) << request.GetURL();
    HandleRequest(session, request);
}

void JSON_RequestHandler::HandleRequest(HttpSessionPtr inSession, Skit::HTTP::Request& inRequest)
{

    Skit::HTTP::Response response;

    response.SetHeader("Accept-Ranges", "bytes");
    // TODO get media type from stream
    response.SetHeader("Content-type", "video/mp4");
    response.SetHeader("Keep-Alive", "timeout=5, max=100");
    response.SetHeader("Connection", "Keep-Alive");

    int theSize = fPlayer->GetSize();
    int theLength = theSize;   // Content length header value
    int theStart = 0;          // Start byte
    int theEnd = theSize - 1;  // End byte

    string theRange = inRequest.GetHeader("Range");
    if (!theRange.empty())
    {
        int rangeStart = theStart;
        int rangeEnd = theEnd;

        size_t pos = theRange.find('=');
        if (string::npos == pos)
        {
            // TODO
            /*
            response.SetHeader("Content-Range", )
            header('HTTP/1.1 416 Requested Range Not Satisfiable');
            header("Content-Range: bytes $start-$end/$size");
            */
        }

        pos++;
        size_t splitPos = theRange.find('-');
        if ( (splitPos != string::npos) && (splitPos > pos) )
        {
            string value = theRange.substr(pos, splitPos - pos); // get start
            rangeStart = atoi(value.c_str());
            splitPos++;
            value = theRange.substr(splitPos, theRange.length() - splitPos); // get end
            rangeEnd = atoi(value.c_str());
            if (rangeEnd)
                rangeEnd = (rangeEnd > theEnd) ? theEnd : rangeEnd;
        }

        if ((rangeStart > rangeEnd)  ||
            (rangeStart > (theSize - 1)) ||
            (rangeEnd >= theSize) )
        {
            // TODO
            /*
            response.SetHeader("Content-Range", )
            header('HTTP/1.1 416 Requested Range Not Satisfiable');
            header("Content-Range: bytes $start-$end/$size");
            */
        }
        theStart = rangeStart;
        if (rangeEnd)
            theEnd = rangeEnd;
        theLength = theEnd - theStart + 1;
    }
    LOG(logERROR) << "seek to :" << theStart;
    if (theStart > 0)
        fPlayer->SeekTo(theStart);

    stringstream ssrange;
    ssrange << "bytes " << theStart << '-' << theEnd << '/' << theSize;
    LOG(logDEBUG) << ssrange.str();

    response.SetHeader("Content-Length", theLength);
    response.SetHeader("Content-Range", ssrange.str());

    if (theRange.empty())
        fResponseHeaders = response.BuildResponse("200", "ОК");
    else
        fResponseHeaders = response.BuildResponse("206", "Partial Content");
    LOG(logDEBUG) << fResponseHeaders;

    Buffer theBuf((void*)fResponseHeaders.c_str(), fResponseHeaders.size());
    inSession->GetSocket()->Send(CreateBufferSequence(theBuf),
                                 BIND_HANDLER(&JSON_RequestHandler::OnSendHandler,
                                 inSession) );
}




