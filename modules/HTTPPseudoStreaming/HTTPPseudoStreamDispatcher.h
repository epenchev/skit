#include <server/PluginModule.h>
#include <server/HTTPServer.h>
#include <stream/StreamClient.h>
#include <HTTP/HTTPRequest.h>
#include "HTTPPseudoStreamSink.h"
#include <map>

class HTTPPseudoStreamDispatcher
 : public PluginObject, public HTTPServerListener,
   public HTTPSessionListener
{
public:
    HTTPPseudoStreamDispatcher();
    virtual ~HTTPPseudoStreamDispatcher() {}

    const char* GetClassID() { return "HTTPPseudoStreamDispatcher"; }

    void OnHTTPSessionAccept(HTTPSessionPtr session);

    void OnHTTPrequest(HTTPSessionPtr session, const HTTPRequest& inRequest);

};
