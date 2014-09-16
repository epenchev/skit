//
// Stream.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef STREAM_H_
#define STREAM_H_

#include <deque>
#include <string>
#include <fstream>

#include "PropertyTree.h"
#include "Buffer.h"
#include "Task.h"

#include <boost/shared_ptr.hpp>

//#define LOG_DISABLE

using namespace std;

// TODO create IStreamReaderSink factory
// TODO create IStreamFilter factory

class IStreamReaderSink
{
    virtual void OnRead(Buffer* inBuf) = 0;
    virtual ~IStreamReaderSink() {};
};

// Gives I/O interface to the stream mostly used by Player instances.
class IStreamReader
{
public:
    virtual Buffer* DoRead() = 0;
    virtual void Seek(long unsigned int byteOffset) = 0;
    virtual void AddListener(IStreamReaderSink* sink) = 0;
    virtual ~IStreamReader() {};
};

class IStreamFilter
{
    // TODO return PlayerAVFrame or NULL if no complete PlayerAVFrame can be extracted from inBuf.
    virtual Buffer* Process(Buffer* inBuf) = 0;
    virtual ~IStreamFilter() {};
};

// Stores Media stream configuration and basic stream management.
class Stream
{
public:
    Stream( PropertyTree::Iterator conf );
    virtual ~Stream();

    // TODO add a method for notification on stream configuration change.
    // Get XML configuration tree iterator
    /*const*/ PropertyTree::Iterator& GetConfig() /*const*/ { return fStreamConfig; }

    IStreamReader* GetReader();
    IStreamFilter* GetFilter();

private:
    PropertyTree::Iterator fStreamConfig;  // stream configuration
};

class Player
{
public:
    Player(ThreadID tid);
    virtual ~Player() {}

    // play a media stream.
    void Play(Stream* inStream);

    // pause accepting data from a stream.
    void Pause();

    // seek to a current location in a stream
    // specified as a bytes offset
    void SeekTo(int bytes);

    // seek to a current location in a stream
    // specified as a time stamp offset in seconds.
    void SeekTo(float timeStamp);

    // TODO return PlayerAVFrame
    Buffer* Get();

    size_t GetSize();

private:
    // push(add) data to player's queue from a stream.
    void PushQueueBuffer(Buffer* data);

    // pop (remove) data from player's queue.
    Buffer* PopQueueBuffer();

    // TODO create default IStreamReader object
    ifstream             fFileReader;
    ThreadID             fThreadId;         // player's owner thread id
    deque<Buffer*>       fPacketQueue;      // packet queue
    Stream*              fStream;           // the stream we are playing
    unsigned long        fSize;


};

#endif /* STREAM_H_ */
