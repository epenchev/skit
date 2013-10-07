#include <iostream>
#include <HTTP/HTTPRequest.h>
#include <server/HTTPServer.h>
//#include <server/PluginModule.h>

class SimpleModule : public HTTPSessionObserver/* , public PluginModule*/
{
public:  
  SimpleModule() {}
  ~SimpleModule() {}
  void OnHTTPrequest(HTTPRequest* inRequest);
  
  void OnModuleLoad();
  void OnModuleUnLoad();
  const char* GetModuleName();
  const char* GetModuleDescription();
  
};

void SimpleModule::OnHTTPrequest(HTTPRequest* inRequest)
{
  if (inRequest)
  {
    std::cout << inRequest->GetProtocol() << std::endl;
    std::cout << inRequest->GetPath() << std::endl;
  }
}

/*
void SimpleModule::OnModuleLoad()
{}

void SimpleModule::OnModuleUnLoad()
{}

const char* SimpleModule::GetModuleDescription()
{ return NULL; }

const char* SimpleModule::GetModuleName()
{ return NULL; }
*/

/*
extern "C" PluginModule* CreateModuleObject()
{
  return new SimpleModule();
}

extern "C" void DestroyModuleObject( PluginModule* object )
{
  delete object;
}
*/

#if 0

int main()
{
  HTTPRequest request;
  std::string stringRequest;
  
  stringRequest = "GET /favicon.ico HTTP/1.1\r\n \
		   Host: 127.0.0.1:8000\r\n \
                   Connection: keep-alive\r\n \
		   Accept: */*\r\n \
	           User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/29.0.1547.76 Safari/537.36\r\n \
		   Accept-Encoding: gzip,deflate,sdch\r\n \
		   Accept-Language: en-US,en;q=0.8\r\n\r\n";
  
  request.Init(stringRequest);
  std::cout << request.GetProtocol() << std::endl;
  std::cout << request.GetPath() << std::endl;
  
  return 0;
}

#endif


