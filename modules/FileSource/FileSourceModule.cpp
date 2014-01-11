#include <server/PluginModule.h>
#include <utils/Logger.h>
#include <stream/StreamFactory.h>
#include "FileSource.h"

/* module name */
static const char* m_moduleName = "FileSourceModule";
/* module description */
static const char* m_moduleDescription = "Module for reading media files for streaming";
/* module author */
static const char* m_moduleAuthor = "Emil Penchev";

static bool m_isLoaded = false;

class FileSourceModule : public PluginModule
{
public:
    FileSourceModule(unsigned id) : m_moduleID(id) {}
    virtual ~FileSourceModule() {}

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

void FileSourceModule::OnModuleLoad()
{
    if (!m_isLoaded)
    {
        FileSource* source = new FileSource();
        StreamFactory::RegisterStreamItem(this, Source, source->GetClassID());
        m_isLoaded = true;
        delete source;
        LOG(logINFO) << "FileSourceModule loaded";
    }
    else
    {
        LOG(logERROR) << "Module "<< m_moduleName << " is already loaded";
    }
}

void FileSourceModule::OnModuleUnLoad()
{
    LOG(logINFO) << "FileSourceModule unloaded";
}

PluginObject* FileSourceModule::CreateInstance()
{
    return new FileSource();
}

void FileSourceModule::DestroyInstance(PluginObject* obj)
{
    FileSource* source = dynamic_cast<FileSource*>(obj);
    if (source)
    {
        delete source;
    }
}

extern "C" PluginModule* CreateModuleObject(unsigned id)
{
    if (!m_isLoaded)
    {
        return new FileSourceModule(id);
    }

    return NULL;
}

extern "C" void DestroyModuleObject( PluginModule* object )
{
    FileSourceModule* module = dynamic_cast<FileSourceModule*>(object);
    if (module)
    {
        delete module;
    }
}
