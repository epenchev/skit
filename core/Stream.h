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
#include "RegFactory.h"

#include <boost/shared_ptr.hpp>

//#define LOG_DISABLE

using namespace std;

// TODO create IStreamSource factory
// TODO create IStreamFilter factory

class IStreamReaderSink
{
public:
    virtual void OnRead(Buffer* inBuf) = 0;
    virtual ~IStreamReaderSink() {};
};

// Gives I/O interface to the stream mostly used by Player instances.
class IStreamSource
{
public:
    static  IStreamSource* CreateItem() { return NULL; }
    virtual void GetSize(unsigned long& outSize) = 0;
    virtual void Open(const string& location) = 0;
    virtual void DoRead(Buffer& outBuf, unsigned long& bytesRead) {};
    virtual void Seek(long unsigned int byteOffset) {};
    // For live streams
    virtual void AddListener(IStreamReaderSink* sink) {};
    virtual ~IStreamSource() {};
};

class IStreamFilter
{
public:
    // TODO return PlayerAVFrame or NULL if no complete PlayerAVFrame can be extracted from inBuf.
    virtual Buffer* Process(Buffer* inBuf) = 0;
    virtual void Init(PropertyTree::Iterator& inOptions) = 0;
    virtual ~IStreamFilter() {};
};

// Stores Media stream configuration and basic stream management.
class Stream
{
public:
    typedef RegFactory<IStreamSource> SourceFactory;
    typedef RegFactory<IStreamFilter> FilterFactory;

    Stream( PropertyTree::Iterator conf );
    virtual ~Stream();

    // TODO add a method for notification on stream configuration change.
    // Get XML configuration tree iterator
    /*const*/ PropertyTree::Iterator& GetConfig() /*const*/ { return fConfig; }

    IStreamSource* GetSource();
    IStreamFilter* GetFilter();

private:
    IStreamSource* fLiveSource;
    PropertyTree::Iterator fConfig;  // stream configuration
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

    ThreadID             fThreadId;         // player's owner thread id
    deque<Buffer*>       fPacketQueue;      // packet queue
    Stream*              fStream;           // the stream we are playing
    IStreamSource*       fSource;
};

#endif /* STREAM_H_ */
