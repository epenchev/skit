#include <server/PluginModule.h>
#include <server/IHTTPServer.h>
#include <HTTP/HTTPRequest.h>
#include <HTTP/HTTPResponse.h>

class HTTPPseudoStream
 : public PluginObject, public HTTPServerListener, public HTTPSessionListener
{
public:
    HTTPPseudoStream();
    virtual ~HTTPPseudoStream() {}

    const char* GetClassID() { return "HTTPPseudoStream"; }

    void OnHTTPSessionCreate(IHTTPSession& session);

    void OnHTTPSessionDestroy(IHTTPSession& session);

    void OnHTTPrequest(const IHTTPSession& session, HTTPRequest& inRequest);

    void OnHTTPRequestReply(const IHTTPSession& session, HTTPResponse& inResponse, bool& forceReply);

    void OnReplySend(IHTTPSession& session, ErrorCode& err);
};
