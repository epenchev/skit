#include <stream/StreamClient.h>
#include <stream/StreamSink.h>
#include <server/ServerController.h>
#include <server/PluginModule.h>
#include <system/Buffer.h>

class SinkHTTPModule : public StreamSink, public PluginModule
{
public:
    SinkHTTPModule() {}
    virtual ~SinkHTTPModule() {}

    /* from PluginModule */
    void OnModuleLoad();

    /* from PluginModule */
    void OnModuleUnLoad();

    /* from PluginModule */
    const char* GetModuleName() { return "HTTPSink"; }

    /* from PluginModule */
    const char* GetModuleDescription() { return "HTTP sink module"; }

    /* from PluginModule */
    const char* GetModuleAuthor() { return "Emil Penchev"; }

    void WriteData(Buffer* data, StreamClient* client);

    /* from StreamSink */
    void AddListener(SinkObserver* listener);

    /* from StreamSink */
    void RemoveListener(SinkObserver* listener);

private:
    ErrorCode m_outError;
    std::set<SinkObserver*> m_listeners; // sink listeners
};

void SinkHTTPModule::OnModuleLoad()
{

}

void SinkHTTPModule::OnModuleUnLoad()
{
    LOG(logINFO) << "Unloading module";
}

void SinkHTTPModule::WriteData(Buffer* data, StreamClient* client)
{

}

void SinkHTTPModule::AddListener(SourceObserver* listener)
{
    if (listener)
    {
        m_listeners.insert(listener);
    }
    else
    {
        LOG(logERROR) << "Invalid listener";
    }
}

void SinkHTTPModule::RemoveListener(SourceObserver* listener)
{
    if (listener)
    {
        if (m_listeners.count(listener) > 0)
        {
            m_listeners.erase(listener);
        }
    }
}

extern "C" PluginModule* CreateModuleObject()
{
  return new SinkHTTPModule();
}

extern "C" void DestroyModuleObject( PluginModule* object )
{
  delete object;
}

