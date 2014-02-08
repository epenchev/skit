#include "FileSource.h"
#include <utils/Logger.h>
#include <utils/PropertyMap.h>
#include <stream/StreamFactory.h>
#include <stream/StreamPlayItem.h>
#include <stream/StreamPacketRec.h>
#include <system/Buffer.h>
#include <system/Task.h>
#include <system/TaskThread.h>
#include <string>

FileSource::FileSource()
{}

FileSource::~FileSource()
{}

void FileSource::Start(Stream& s)
{
    LOG(logDEBUG) << "Starting Source";
    PropertyMap& pmap = s.GetProperties();
    std::string filename = pmap.GetProperty("filename");
    if (!filename.empty())
    {
        if (m_reader.Open(filename))
        {
            LOG(logDEBUG) << "File opened successfully, notify listeners starting source";
            pmap.SetProperty<long long>("filesize", m_reader.GetSize());
            pmap.SetProperty<unsigned long>("mtime", m_reader.LastWriteTime());
        }
        else
        {
            m_error.SetValue(-1);
            m_error.SetMessage("Error opening file");
            LOG(logERROR) << "Error opening " << filename;
        }
    }
    else
    {
        m_error.SetValue(-1);
        m_error.SetMessage("Invalid filename");
        LOG(logERROR) << "Invalid filename";
    }

    SystemMutexLocker lock(m_lockListeners);
    for (std::set<SourceObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        Task* evtask = new Task();
        evtask->Connect(&SourceObserver::OnStart, *it, boost::ref(*this), boost::ref(m_error));
        TaskThreadPool::Signal(evtask);
    }

}

void FileSource::Stop()
{
    LOG(logDEBUG) << "We are stopping, notify listeners";
    m_reader.Close();
    SystemMutexLocker lock(m_lockListeners);
    for (std::set<SourceObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        Task* evtask = new Task();
        evtask->Connect(&SourceObserver::OnStop, *it, boost::ref(*this));
        TaskThreadPool::Signal(evtask);
    }
}

void FileSource::Seek(StreamPlayItem& playItem /*unsigned position, unsigned length*/)
{
    Buffer* buf = NULL;

    unsigned long position = playItem.GetStart();
    unsigned long length =  playItem.GetLength();

    LOG(logDEBUG) << "Seek to position:" << position << " and length:" << length;
    if (m_reader.Seek(position))
    {
        unsigned buflen = ( length != 0 ? length : FileSource::m_defaultDataSize );
        char* data = new char[buflen];
        int bytesRead = m_reader.Read((void*)data, buflen);
        if (-1 != bytesRead)
        {
            buf = new Buffer(data, bytesRead);
            LOG(logDEBUG) << "Read " << bytesRead << " from " << m_reader.GetFileName();
        }
        else
        {
            m_error.SetValue(-1);
            m_error.SetMessage("Error reading from file");
            LOG(logERROR) << m_error.GetErrorMessage();
        }
    }
    else
    {
        m_error.SetValue(-1);
        m_error.SetMessage("Unable to seek to given position");
        LOG(logERROR) << "Unable to seek to given position:" << position;
    }

    LOG(logDEBUG) << "Listeners count " << m_listeners.size();

    StreamPacketRec* packet = new StreamPacketRec(*buf, playItem);
    SystemMutexLocker lock(m_lockListeners);
    for (std::set<SourceObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        LOG(logDEBUG) << "Notify listeners";
        Task* evtask = new Task();
        evtask->Connect(&SourceObserver::OnDataReceive, *it, boost::ref(*this), boost::ref(*packet), boost::ref(m_error));
        TaskThreadPool::Signal(evtask);
    }
}

bool FileSource::IsSeekable()
{
    return true;
}

void FileSource::AddListener(SourceObserver* listener)
{
    if (listener)
    {
        SystemMutexLocker lock(m_lockListeners);
        LOG(logDEBUG) << "Listener added";
        m_listeners.insert(listener);
    }
}

void FileSource::RemoveListener(SourceObserver* listener)
{
    if (listener)
    {
        SystemMutexLocker lock(m_lockListeners);
        LOG(logDEBUG) << "Listener removed";
        m_listeners.erase(listener);
    }
}
