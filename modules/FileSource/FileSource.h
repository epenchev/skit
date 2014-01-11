#include <server/PluginModule.h>
#include <stream/StreamSource.h>
#include <stream/Stream.h>
#include <utils/FileReader.h>
#include <utils/ErrorCode.h>
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

    void Seek(unsigned position, unsigned length = 0);

    bool IsSeekable();

    void AddListener(SourceObserver* listener);

    void RemoveListener(SourceObserver* listener);
private:
    ErrorCode m_error;
    FileReader m_reader;
    std::set<SourceObserver*> m_listeners;
};
