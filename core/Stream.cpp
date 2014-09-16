//
// Stream.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "Stream.h"
//#define LOG_DISABLE
#include "Logger.h"


using namespace std;

Stream::Stream( PropertyTree::Iterator conf )
: fLiveSource(NULL)
{
    fConfig = conf;
}

Stream::~Stream()
{}

IStreamSource* Stream::GetSource()
{
    string sourceClass;
    IStreamSource* theSource = NULL;

    if (fConfig.GetData<bool>("live", false))
    {
        if (!fLiveSource)
        {
            /* for live streams source must be always present in the configuration */
            sourceClass = fConfig.GetData("source");
            if (sourceClass.empty())
                return theSource;
        }
        else
            return fLiveSource;
    }

    /*
     * FileReader class is always the default IStreamSource implementation used for a source
     * when no such is specified in the stream configuration.
     */
    if (sourceClass.empty())
        theSource = Stream::SourceFactory::CreateInstance("FileReader");
    else
        theSource = Stream::SourceFactory::CreateInstance(sourceClass);
    //theSource->Open(fConfig.GetData("location")); // TODO consider using Init() method here

    return theSource;
}

IStreamFilter* Stream::GetFilter()
{
    IStreamFilter* theFilter = NULL;
    string filterClass = fConfig.GetData("filter");
    if (!filterClass.empty())
    {
        theFilter = Stream::FilterFactory::CreateInstance(filterClass);
        theFilter->Init(fConfig);
    }

    return theFilter;
}

Player::Player(ThreadID tid)
 : fThreadId(tid), fStream(NULL), fSource(NULL)
{}

void Player::PushQueueBuffer(Buffer* data)
{
    if (data)
    {
        /* push data to queue only from this thread (player's context) */
        if (boost::this_thread::get_id() == fThreadId)
            fPacketQueue.push_back(data);
        else
        {
            TaskScheduler* scheduler = &TaskScheduler::Instance();
            scheduler->QueueTask( Task::Connect( &Player::PushQueueBuffer, this, data ), fThreadId );
        }
    }
}

Buffer* Player::PopQueueBuffer()
{
    Buffer* packet = NULL;

    /*
     * pop data from queue only from this thread (player's context),
     * PopQueueBuffer() is meant to be called directly from Player's owner who actually assigned fThreadId
     */

    if (boost::this_thread::get_id() == fThreadId)
    {
        if (!fPacketQueue.empty())
        {
            packet = fPacketQueue.front();
            fPacketQueue.pop_front();
        }
        else
        {
            LOG(logWARNING) << "Queue empty";
        }
    }
    return packet;
}

void Player::Play(Stream* inStream)
{
    if (inStream)
    {
        fStream = inStream;
        fSource = fStream->GetSource();
        bool islive = fStream->GetConfig().GetData<bool>("live", false);
        if (islive)
        {
            //theSource->AddListener(this);
        }
        else
        {
            string theLocation = fStream->GetConfig().GetData("location");
            fSource->Open(theLocation);
        }
    }
}

void Player::Pause()
{
    // TODO implement
}

void Player::SeekTo(int inOffset)
{
    LOG(logDEBUG) << "Here : inOffset:" << inOffset;
    fSource->Seek(inOffset);
    // TODO log error
    // TODO flush remaining data and free memory before pushing to queue
    //fPacketQueue.clear();
    for (int idx = 0; idx < 3; idx++)
    {
        /*
        * Do some pre buffering before play.
        */
        Buffer* outPacket = new Buffer(5000);
        unsigned long bytesRead;
        fSource->DoRead(*outPacket, bytesRead);
        PushQueueBuffer(outPacket);
    }
}

Buffer* Player::Get()
{
    unsigned long bytesRead;

    Buffer* outPacket = new Buffer(5000);
    fSource->DoRead(*outPacket, bytesRead);
    PushQueueBuffer(outPacket);
    return PopQueueBuffer();
}

size_t Player::GetSize()
{
    unsigned long int theSize;
    fSource->GetSize(theSize);

    return theSize;
}

void Player::SeekTo(float timeStamp)
{
    // TODO implement
}


