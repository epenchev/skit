#include <set>
#include <iostream>
#include <stream/StreamSource.h>
#include <server/ServerController.h>
#include <server/PluginModule.h>
#include <system/Task.h>
#include <system/TaskThread.h>
#include <system/Buffer.h>
#include <FileReader.h>
#include <Logger.h>
#include <ErrorCode.h>

class SourceModule : public StreamSource, public PluginModule
{
public:
    SourceModule() : m_readBuf(NULL) {}
    virtual ~SourceModule() { delete m_readBuf; }

    /* from PluginModule */
    void OnModuleLoad();

    /* from PluginModule */
    void OnModuleUnLoad();

    /* from PluginModule */
    const char* GetModuleName() { return "Empty SourceModule"; }

    /* from PluginModule */
    const char* GetModuleDescription() { return "Simple source module"; }

    /* from PluginModule */
    const char* GetModuleAuthor() { return "Emil Penchev"; }

    /* from StreamSource */
    void Start();

    /* from StreamSource */
    void Stop();

    /* from StreamSource */
    void Seek(int position);

    /* from StreamSource */
    bool IsSeekable();

    /* from StreamSource */
    void AddListener(SourceObserver* listener);

    /* from StreamSource */
    void RemoveListener(SourceObserver* listener);

private:
    ErrorCode m_outError;
    std::set<SourceObserver*> m_listeners; // source listeners
    blitz::FileReader m_reader;
    Buffer* m_readBuf;
};

void SourceModule::OnModuleLoad()
{
    std::cout << ServerController::GetStreamCount() << std::endl;
    m_readBuf = new Buffer(new char[1024], 1024);

    if (m_readBuf)
    {
        if (!m_reader.open("/home/emo/Downloads/oceans-clip.mp4"))
        {
            LOG(logERROR) << "Error opening file";
        }
        else
        {
            LOG(logINFO) << "File opened";
        }
    }
}

void SourceModule::Start()
{
    //LOG(logDEBUG) << "Starting source ";
    if (m_reader.is_open())
    {
        //LOG(logDEBUG) << "buffer clear ";
        m_readBuf->Clear();
        //LOG(logDEBUG) << "reading from file ";
        int bytesRead = m_reader.read(m_readBuf->Get(), m_readBuf->Size());
        //LOG(logDEBUG) << "Bytes read " << bytesRead;
        if (bytesRead != -1)
        {
            // notify listeners for incoming data
            for (std::set<SourceObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
            {
                //LOG(logDEBUG) << "Notify observers";
                SourceObserver* listener = *it;
                Task* eventTask = new Task();
                eventTask->Connect(&SourceObserver::OnDataReceive, listener, this, m_readBuf, &m_outError);
                TaskThreadPool::Signal(eventTask);
            }
        }
        else
        {
            Stop();
        }
        /*
            if (m_reader.is_eof())
            {
                LOG(logWARNING) << "Got EOF bit set from file";
                m_outError.SetValue(-1);
                m_outError.SetMessage("EOF bit set from file");
            }
            this->Stop();
        }
        else
        {

        }*/
    }
}

void SourceModule::Seek(int position)
{
	m_reader.seekInFile(position);
}

void SourceModule::AddListener(SourceObserver* listener)
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

void SourceModule::RemoveListener(SourceObserver* listener)
{
    if (listener)
    {
        if (m_listeners.count(listener) > 0)
        {
            m_listeners.erase(listener);
        }
    }
}

void SourceModule::Stop()
{
    LOG(logWARNING) << "Closing file";
    m_reader.close();
    // notify listeners for Stoping
    for (std::set<SourceObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {

        SourceObserver* listener = *it;
        Task* eventTask = new Task();
        eventTask->Connect(&SourceObserver::OnStop, listener, this);
        TaskThreadPool::Signal(eventTask);
    }
}

bool SourceModule::IsSeekable()
{
    return true;
}

void SourceModule::OnModuleUnLoad()
{
    LOG(logINFO) << "Unloading module";
    m_reader.close();
}

extern "C" PluginModule* CreateModuleObject()
{
  return new SourceModule();
}

extern "C" void DestroyModuleObject( PluginModule* object )
{
  delete object;
}





