#include <iostream>
#include <stream/StreamSource.h>
#include <server/ServerController.h>
#include <server/PluginModule.h>

class SourceModule : public StreamSource, public PluginModule
{
public:
    SourceModule() {}
    virtual ~SourceModule() {}

    void OnModuleLoad();

    void OnModuleUnLoad();

    const char* GetModuleName();

    const char* GetModuleDescription();

    const char* GetModuleAuthor();

    void Start() { std::cout << "Start() \n"; }
    void Stop() {}
    void Seek() {}
    bool IsSeekable() { return false; }
    void AddListener(SourceObserver* listener) {}
    void RemoveListener(SourceObserver* listener) {}

/*
    void WriteData(Buffer* data, StreamClient* client) {}
    void AddListerner(SinkObserver* listener) {}
    void RemoveListerner(SinkObserver* listener) {}
  */
};

void SourceModule::OnModuleLoad()
{
    std::cout << "OnModuleLoad() called \n";
    std::cout << ServerController::GetStreamCount() << std::endl;
    //std::cout << "Register Source \n";
    //this->Start();
}

void SourceModule::OnModuleUnLoad()
{}

const char* SourceModule::GetModuleDescription()
{
    // std::cout << ServerController::GetStreamCount() << std::endl;
    return "Simple source module";
}

const char* SourceModule::GetModuleName()
{
    return "Empty SourceModule";
}

const char* SourceModule::GetModuleAuthor()
{
    return "Emil Penchev";
}


extern "C" PluginModule* CreateModuleObject()
{
  return new SourceModule();
}

extern "C" void DestroyModuleObject( PluginModule* object )
{
  delete object;
}





