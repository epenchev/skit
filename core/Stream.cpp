//
// Stream.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "Stream.h"
//#define LOG_DISABLE
#include "Logger.h"


using namespace std;

Stream::Stream( PropertyTree::Iterator conf )
{
    fStreamConfig = conf;
}

Stream::~Stream()
{}

IStreamReader* Stream::GetReader()
{
    return NULL;
}

IStreamFilter* Stream::GetFilter()
{
    return NULL;
}

Player::Player(ThreadID tid)
 : fThreadId(tid), fStream(NULL), fSize(0)
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
        if (!fStream->GetReader())
        {
            string theLocation = fStream->GetConfig().GetData("location");
            if (!theLocation.empty())
            {
                fFileReader.open(theLocation.c_str(), ifstream::binary | ifstream::in);
                fFileReader.seekg(0, ios_base::end);
                fSize = fFileReader.tellg();
                fFileReader.seekg(0, ios_base::beg);

                //Buffer* outPacket = new Buffer(500);
                //fFileReader.read(outPacket->Get<char*>(), outPacket->Size());
                //PushQueueBuffer(outPacket);

#if 0
                for (int idx = 0; idx < 10; idx++)
                {
                    /*
                     * Do some pre buffering before play.
                     */
                    Buffer* outPacket = new Buffer(500);
                    fFileReader.read(outPacket->Get<char*>(), outPacket->Size());
                    PushQueueBuffer(outPacket);
                }
#endif
            }

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
    if (fFileReader.is_open())
    {
        //Buffer* outPacket = new Buffer(5000);
        fFileReader.seekg(inOffset, ios_base::beg);
        //fFileReader.read(outPacket->Get<char*>(), outPacket->Size());
        // TODO log error
        // LOG(logERROR) << "Error reading packet from file: "<< fStreamConfig.GetData("location");
        // TODO flush remaining data and free memory before pushing to queue
        //fPacketQueue.clear();
        for (int idx = 0; idx < 3; idx++)
        {
             /*
              * Do some pre buffering before play.
              */
             Buffer* outPacket = new Buffer(5000);
             fFileReader.read(outPacket->Get<char*>(), outPacket->Size());
             PushQueueBuffer(outPacket);
         }
    }
}

Buffer* Player::Get()
{
    Buffer* outPacket = new Buffer(5000);
    fFileReader.read(outPacket->Get<char*>(), outPacket->Size());
    PushQueueBuffer(outPacket);
    return PopQueueBuffer();
    //return outPacket;
}

size_t Player::GetSize()
{
    return fSize;
}

void Player::SeekTo(float timeStamp)
{
    // TODO implement
}


