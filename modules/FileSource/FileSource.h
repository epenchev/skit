#include <server/PluginModule.h>
#include <stream/StreamSource.h>
#include <stream/Stream.h>
#include <utils/FileReader.h>
#include <utils/ErrorCode.h>
#include <system/SystemThread.h>
#include <set>

class FileSource
 : public PluginObject, public StreamSource
{
public:
    FileSource();
    virtual ~FileSource();

    const char* GetClassID() { return "FileSource"; }

    void Start(Stream& s);

    void Stop();

    void Seek(StreamPlayItem& playItem);

    bool IsSeekable();

    void AddListener(SourceObserver* listener);

    void RemoveListener(SourceObserver* listener);
private:
    static const unsigned m_defaultDataSize = 100;
    ErrorCode m_error;
    FileReader m_reader;
    SystemMutex  m_lockListeners; /**< lock listeners set, provides thread safety when adding/removing listeners */
    std::set<SourceObserver*> m_listeners;
};
