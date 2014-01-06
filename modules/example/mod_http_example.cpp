#include <server/PluginModule.h>
#include <utils/Logger.h>
#include "HTTPPseudoStream.h"

/* module name */
static const char* m_moduleName = "ExampleHTTPModule";
/* module description */
static const char* m_moduleDescription = "Example HTTP module for educational purpose.";
/* module author */
static const char* m_moduleAuthor = "Emil Penchev";

class ExampleHTTPModule : public PluginModule
{
public:
	ExampleHTTPModule(unsigned id) : m_moduleID(id) {}
	virtual ~ExampleHTTPModule() {}

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
	//HTTPPseudoStream* m_pluginObj;
};

void ExampleHTTPModule::OnModuleLoad()
{
	LOG(logINFO) << "ExampleHTTPModule loaded";
	CreateInstance();
}

void ExampleHTTPModule::OnModuleUnLoad()
{
	LOG(logINFO) << "ExampleHTTPModule unloaded";
}

PluginObject* ExampleHTTPModule::CreateInstance()
{
	return new HTTPPseudoStream();
}

void ExampleHTTPModule::DestroyInstance(PluginObject* obj)
{
	delete obj;
}

extern "C" PluginModule* CreateModuleObject(unsigned id)
{
  return new ExampleHTTPModule(id);
}

extern "C" void DestroyModuleObject( PluginModule* object )
{
	HTTPPseudoStream* p_obj = (HTTPPseudoStream*)object;
	delete p_obj;
}
