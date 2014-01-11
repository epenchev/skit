#include <server/PluginModule.h>
#include <utils/Logger.h>
#include "HTTPPseudoStream.h"

/* module name */
static const char* m_moduleName = "ExampleHTTPModule";
/* module description */
static const char* m_moduleDescription = "Example HTTP module for educational purpose.";
/* module author */
static const char* m_moduleAuthor = "Emil Penchev";

class HTTPPseudoStreamModule : public PluginModule
{
public:
    HTTPPseudoStreamModule(unsigned id) : m_moduleID(id) {}
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
};

void HTTPPseudoStreamModule::OnModuleLoad()
{
    LOG(logINFO) << "ExampleHTTPModule loaded";
    CreateInstance();
}

void HTTPPseudoStreamModule::OnModuleUnLoad()
{
    LOG(logINFO) << "ExampleHTTPModule unloaded";
}

PluginObject* HTTPPseudoStreamModule::CreateInstance()
{
    return new HTTPPseudoStream();
}

void HTTPPseudoStreamModule::DestroyInstance(PluginObject* obj)
{
    HTTPPseudoStream* p_obj = dynamic_cast<HTTPPseudoStream*>(obj);
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
