//
// StreamFileSource.h
// Copyright (C) 2014  Emil Penchev, Bulgaria

#ifndef STREAM_FILE_SOURCE_H_
#define STREAM_FILE_SOURCE_H_

#include "Stream.h"
#include <string>
#include <fstream>

using namespace std;

class StreamFileSource : public IStreamSource
{
public:
    static IStreamSource* CreateItem() { return new StreamFileSource; }
    StreamFileSource();
    void GetSize(unsigned long& outSize) { outSize = fFileSize; }
    void Open(const string& location);
    void DoRead(Buffer& outBuf, unsigned long& bytesRead);
    void Seek(long unsigned int byteOffset);
private:
    ifstream       fFileHandle;
    unsigned long  fFileSize;
};

#endif /* STREAM_FILE_SOURCE_H_ */
