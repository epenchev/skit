//
// StreamFileSource.cpp
// Copyright (C) 2014  Emil Penchev, Bulgaria

#include "StreamFileSource.h"
#include "ErrorCode.h"
#include "Logger.h"

static Stream::SourceFactory::Registrator<StreamFileSource> reg("FileReader");

StreamFileSource::StreamFileSource() : fFileSize(0)
{
    LOG(logDEBUG) << "Created Reader";
}

void StreamFileSource::Open(const string& location)
{
    ErrorCode err;
    if (location.empty())
    {
        err.SetMessage("Error opening file, empty location");
        throw SystemException(err);
    }

    fFileHandle.open(location.c_str(), ifstream::binary | ifstream::in);
    if (fFileHandle.fail())
    {
        err.SetMessage("Error opening file location:" + location);
        throw SystemException(err);
    }
    fFileHandle.seekg(0, ios_base::end);
    fFileSize = fFileHandle.tellg();
    fFileHandle.seekg(0, ios_base::beg);
}

void StreamFileSource::DoRead(Buffer& outBuf, unsigned long& bytesRead)
{
    bytesRead = 0; // indicates error
    if (fFileHandle.is_open())
    {
        if (!fFileHandle.eof())
        {
            fFileHandle.read(outBuf.Get<char*>(), outBuf.Size());
            bytesRead = fFileHandle.gcount();
        }
    }
}

void StreamFileSource::Seek(long unsigned int byteOffset)
{
    ErrorCode err;
    if (fFileHandle.is_open())
    {
        if (byteOffset > fFileSize)
        {
            err.SetMessage("offset exceeds range");
            throw SystemException(err);
        }

        fFileHandle.seekg(byteOffset);
        if (fFileHandle.fail())
        {
            err.SetMessage("error seeking in file");
            throw SystemException(err);
        }
    }
}



