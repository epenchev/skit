#include <server/PluginModule.h>
#include <utils/Logger.h>
#include <stream/StreamFactory.h>
#include "HTTPPseudoStreamDispatcher.h"
#include "HTTPPseudoStreamSink.h"

/* module name */
static const char* m_moduleName = "HTTPPseudoStreaming";
/* module description */
static const char* m_moduleDescription = "HTTP pseudo streaming module.";
/* module author */
static const char* m_moduleAuthor = "Emil Penchev";

class HTTPPseudoStreamModule : public PluginModule
{
public:
    HTTPPseudoStreamModule(unsigned id) : m_moduleID(id), m_dispatcher(NULL) {}
    virtual ~HTTPPseudoStreamModule() {}

    /* from PluginModule */
    void OnModuleLoad();

    /* from PluginModule */
    void OnModuleUnLoad();

    /* from PluginModule */
    const char* GetModuleName() { return m_moduleName; }

    /* from PluginModule */
    const char* GetModuleDescription() { return m_moduleDescription; }

    /* from PluginModule */
    const char* GetModuleAuthor() { return m_moduleAuthor; }

    /* from PluginModule */
    unsigned GetModuleID() { return m_moduleID; }

    /* from PluginModule */
    PluginObject* CreateInstance();

    /* from PluginModule */
    void DestroyInstance(PluginObject* obj);

private:
    unsigned m_moduleID;
    HTTPPseudoStreamDispatcher* m_dispatcher;
};

void HTTPPseudoStreamModule::OnModuleLoad()
{
    // create dispatcher singleton
    m_dispatcher = new HTTPPseudoStreamDispatcher();

    HTTPPseudoStreamSink* sink = new HTTPPseudoStreamSink();
    StreamFactory::RegisterStreamItem(this, Sink, sink->GetClassID());
    delete sink;
    LOG(logINFO) << "HTTPPseudoStreamModule loaded";

}

void HTTPPseudoStreamModule::OnModuleUnLoad()
{
    LOG(logINFO) << "ExampleHTTPModule unloaded";
}

PluginObject* HTTPPseudoStreamModule::CreateInstance()
{
    LOG(logDEBUG) << "{} enter";
    HTTPPseudoStreamSink* sink = new HTTPPseudoStreamSink();

    if (m_dispatcher)
    {
        LOG(logDEBUG) << m_dispatcher->GetClassID();
        //m_dispatcher->NotifyCreateSink(sink);
    }

    LOG(logDEBUG) << "{} exit";
    return sink;
}

void HTTPPseudoStreamModule::DestroyInstance(PluginObject* obj)
{
    HTTPPseudoStreamDispatcher* p_obj = dynamic_cast<HTTPPseudoStreamDispatcher*>(obj);
    if (p_obj)
    {
        delete p_obj;
    }
}

extern "C" PluginModule* CreateModuleObject(unsigned id)
{
  return new HTTPPseudoStreamModule(id);
}

extern "C" void DestroyModuleObject( PluginModule* object )
{
    HTTPPseudoStreamModule* module = dynamic_cast<HTTPPseudoStreamModule*>(object);
    if (module)
    {
        delete module;
    }
}
