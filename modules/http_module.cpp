#include <iostream>
#include <HTTP/HTTPRequest.h>
#include <server/IHTTPServer.h>
#include <server/PluginModule.h>
#include <server/ServerController.h>
#include <server/NetConnection.h>
#include <stream/StreamClient.h>

class SimpleModule : public HTTPSessionObserver, public HTTPServerObserver, public PluginModule
{
public:
  SimpleModule() : m_stream(NULL), m_stream_played(false) {}
  ~SimpleModule() {}

  void OnHTTPrequest(IHTTPSession* session, HTTPRequest* inRequest);
  void OnReplySend(IHTTPSession* session);

  void OnHTTPSessionCreate(IHTTPSession* session);
  void OnHTTPSessionDestroy(IHTTPSession* session);
  void OnServerStart(IHTTPServer* server);
  void OnServerStop(IHTTPServer* server);

  void OnModuleLoad();
  void OnModuleUnLoad();
  const char* GetModuleName();
  const char* GetModuleDescription();
  const char* GetModuleAuthor();

  std::set<StreamClient*> m_clients;
  Stream* m_stream;
  bool err_404_send; // todo remove
  bool m_stream_played;
};


void SimpleModule::OnHTTPrequest(IHTTPSession* session, HTTPRequest* inRequest)
{
  if (inRequest)
  {
	  err_404_send = false; // todo remove
      std::cout << inRequest->GetPath() << std::endl;

      if (inRequest->GetPath().compare("oceans-clip.mp4") == 0)
      {
    	  // Create the stream
    	  if (NULL == m_stream)
    	  {
    		  unsigned streamid = ServerController::CreateStream("file");
    	      std::cout << "!!!!! STREAM Create stream !!!!\n";
    	      Stream* s = ServerController::GetStream(streamid);
    	      s->SetSource(ServerController::GetFileSource());
    	      m_stream = s;
    	  }

    	  // Subscribe client
    	  NetConnection* conn = dynamic_cast<NetConnection*>(session);
    	  for (std::set<StreamClient*>::iterator it=m_clients.begin(); it!=m_clients.end(); ++it)
    	  {
    		  StreamClient* cl = *it;
    		  std::cout << "Shit 1\n";
    		  NetConnection* clientConn = *cl->GetConnections().begin();
    		  std::cout << "Shit 2\n";
    		  if (clientConn->GetConnId() == conn->GetConnId())
    		  {
    			  if (!cl->IsSubscribed())
    			  {
    				  if (m_stream)
    				  {
    					  cl->Subscribe(m_stream->GetStreamId());
    				  }
    			  }
    		  }
    	  }

    	  std::string range = inRequest->GetHeader("Range");
    	  std::string fromRange;
          if (!range.empty()) // Range request
          {
        	  std::cout << range << std::endl;
        	  std::size_t pos = range.find_first_of('=');
        	  if (pos != std::string::npos)
        	  {
        		  std::size_t rangeSplit = range.find_first_of('-', pos+1);
        		  if (rangeSplit != std::string::npos)
        		  {
        			  fromRange = range.substr(pos+1, (rangeSplit - 1) - (pos));
        			  std::cout << "fromRange=" << fromRange << std::endl;
        		  }
        	  }

        	  std::string responseData;
        	  HTTPHeadersMap mapheaders;

        	  mapheaders.insert(HTTPParam("Content-Type", "video/mp4"));
        	  mapheaders.insert(HTTPParam("Last-Modified", "Wed,15 Nov 2013 20:22:23 GMT"));
        	  mapheaders.insert(HTTPParam("ETag", "340ad-128c1b1-4eb236a05f280"));
        	  mapheaders.insert(HTTPParam("Accept-Ranges", "bytes"));
        	  mapheaders.insert(HTTPParam("Content-Length", "19448240")); // -1 EOF
        	  std::string headerRange = "bytes " + fromRange + "-19448240/19448241";
        	  mapheaders.insert(HTTPParam("Content-Range",headerRange /*bytes 0-19448240/19448241"*/));
        	  //mapheaders.insert(HTTPParam("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0"));
        	  //mapheaders.insert(HTTPParam("Pragma", "no-cache"));
        	  //mapheaders.insert(HTTPParam("Keep-Alive", "timeout=5, max=100"));
        	  mapheaders.insert(HTTPParam("Connection", "Keep-Alive"));
        	  std::string httpResponse = HTTPUtils::HTTPResponseToString(206, NULL, &mapheaders);
        	  std::cout << httpResponse << std::endl;

        	  session->Reply(this, httpResponse);
        	  m_stream->Seek(atoi(fromRange.c_str()));
          }
          else  // Not a range request
          {
        	  std::string responseData;
        	  HTTPHeadersMap mapheaders;

        	  mapheaders.insert(HTTPParam("Content-Type", "video/mp4"));
        	  mapheaders.insert(HTTPParam("Last-Modified", "Wed,15 Nov 2013 20:22:23 GMT"));
        	  mapheaders.insert(HTTPParam("ETag", "340ad-128c1b1-4eb236a05f280"));
        	  //mapheaders.insert(HTTPParam("Accept-Ranges", "bytes"));
        	  mapheaders.insert(HTTPParam("Content-Length", "19448241"));
        	  //mapheaders.insert(HTTPParam("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0"));
        	  //mapheaders.insert(HTTPParam("Pragma", "no-cache"));
        	  //mapheaders.insert(HTTPParam("Keep-Alive", "timeout=5, max=100"));
        	  mapheaders.insert(HTTPParam("Connection", "Keep-Alive"));
        	  std::string httpResponse = HTTPUtils::HTTPResponseToString(200, &responseData, &mapheaders);
        	  std::cout << httpResponse << std::endl;

        	  session->Reply(this, httpResponse);
        	  //m_stream->Seek(0);
        	  m_stream->Play();
          }
      }
      else
      {
    	  err_404_send = true;
    	  HTTPHeadersMap mapheaders;
    	  std::string responseData = "<html><body><h2>HTTP 404 Not found</h2></body></html>";
    	  std::string httpResponse = HTTPUtils::HTTPResponseToString(404, &responseData, NULL);
    	  std::cout << httpResponse << std::endl;
    	  session->Reply(this, httpResponse);
      }
  }

}

void SimpleModule::OnReplySend(IHTTPSession* session)
{
    if (session)
    {
        NetConnection* conn = dynamic_cast<NetConnection*>(session);
        if (session->GetRequest().GetPath().compare("favicon.ico") == 0)
        {
        	//conn->Close();
        }
        else
        {
        	std::cout << "Start the stream \n";
        	if (m_stream)
        	{
        		if (!m_stream_played)
        		{
        			m_stream_played = true;
        			m_stream->Play();
        		}
        		std::cout << "Stream is started \n";
        	}
        }
    }
}

void SimpleModule::OnModuleLoad()
{
    IHTTPServer* server = ServerController::GetHTTPServer();
    if (server)
    {
        server->AddServerListener(this);
    }
}

void SimpleModule::OnModuleUnLoad()
{}

const char* SimpleModule::GetModuleDescription()
{ return NULL; }

const char* SimpleModule::GetModuleName()
{ return NULL; }

const char* SimpleModule::GetModuleAuthor()
{ return NULL; }


void SimpleModule::OnHTTPSessionCreate(IHTTPSession* session)
{
    if (session)
    {
        session->AddHTTPSessionListener(this);

        std::cout << "!!! new connection in request handler, create client !!!!\n";
        StreamClient* client = new StreamClient();
        NetConnection* conn = dynamic_cast<NetConnection*>(session);
        if (conn)
        {
        	client->Register(conn);
        }
        session->AcceptRequest();
        m_clients.insert(client);
    }
}

void SimpleModule::OnHTTPSessionDestroy(IHTTPSession* session)
{
	NetConnection* connection = dynamic_cast<NetConnection*>(session);
	if (connection)
	{
		for (std::set<StreamClient*>::iterator it=m_clients.begin(); it!=m_clients.end(); ++it)
		{
			StreamClient* cl = *it;
			NetConnection* clientConn = *cl->GetConnections().begin();
			if (clientConn->GetConnId() == connection->GetConnId())
			{
				m_clients.erase(cl);
				//cl->UnSubscribe(m_stream->GetStreamId());

			}
		}
	    std::cout << "Session Destroy remove client !!!!\n";
	}
}

void SimpleModule::OnServerStart(IHTTPServer* server)
{}

void SimpleModule::OnServerStop(IHTTPServer* server)
{}


extern "C" PluginModule* CreateModuleObject()
{
  return new SimpleModule();
}

extern "C" void DestroyModuleObject( PluginModule* object )
{
  delete object;
}
