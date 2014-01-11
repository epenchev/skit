#include "FileSource.h"
#include <utils/Logger.h>
#include <utils/PropertyMap.h>
#include <stream/StreamFactory.h>
#include <system/Buffer.h>
#include <system/Task.h>
#include <system/TaskThread.h>

FileSource::FileSource()
{}

FileSource::~FileSource()
{}

void FileSource::Start(Stream& s)
{
    if (StreamFactory::IsPublished(s.GetStreamID()))
    {
        LOG(logDEBUG) << "Stream is published so starting ..";
        PropertyMap& pmap = s.GetProperties();
        pmap.SetProperty("filename", "bbb_sunflower.mp4");
	if (m_reader.Open("/media/20A7503C234281D8/bbb_sunflower.mp4"))
	{
	    LOG(logDEBUG) << "File opened successfuly";
	    pmap.SetProperty<long long>("filesize", m_reader.GetSize());
	    // Notify listeners
	    for (std::set<SourceObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
	    {
		Task* evtask = new Task();
		evtask->Connect(&SourceObserver::OnStart, *it, boost::ref(*this));
		TaskThreadPool::Signal(evtask);
	    }
	}
    }
}

void FileSource::Stop()
{
    LOG(logDEBUG) << "We are stopping ..";
    m_reader.Close();
    // Notify listeners
    for (std::set<SourceObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
	Task* evtask = new Task();
	evtask->Connect(&SourceObserver::OnStop, *it, boost::ref(*this));
	TaskThreadPool::Signal(evtask);
    }
}

void FileSource::Seek(unsigned position, unsigned length)
{
    LOG(logDEBUG) << "Seek to position:" << position << " and length:" << length;
    if (m_reader.Seek(position))
    {
	unsigned buflen = ( length != 0 ? length : 100 );
	char* data = new char[buflen];
	int bytesRead = m_reader.Read((void*)data, buflen);
	if (-1 != bytesRead)
	{
	    Buffer* buf = new Buffer(data, bytesRead);
	    // Notify listeners
	    for (std::set<SourceObserver*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
	    {
		Task* evtask = new Task();
		evtask->Connect(&SourceObserver::OnDataReceive, *it, boost::ref(*this), buf, boost::ref(m_error));
		TaskThreadPool::Signal(evtask);
	    }
	}
    }
    else
    {
	LOG(logERROR) << "Unable to seek to given position:" << position;
    }
}

bool FileSource::IsSeekable()
{
    return true;
}

// TODO add mutex
void FileSource::AddListener(SourceObserver* listener)
{
    if (listener)
    {
        LOG(logDEBUG) << "Listener added";
        m_listeners.insert(listener);
    }
}

void FileSource::RemoveListener(SourceObserver* listener)
{
    if (listener)
    {
        LOG(logDEBUG) << "Listener removed";
        m_listeners.erase(listener);
    }
}
