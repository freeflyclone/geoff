#include "ClientNet.h"

ClientNet::ClientNet()
{
    //this->ws_ = ws_;
}

void ClientNet::initialise(int isLittleEndian)
{
    this->isLittleEndian = isLittleEndian;
    framesWithoutContact = 0;
}

AppBuffer::AppBuffer(int bufferLength, int isLittleEndian)
{
    this->bufferLength = bufferLength;
    this->isLittleEndian = isLittleEndian;

    tempBuf = new UINT8[bufferLength];
    writeOffset = 0;
}

AppBuffer::~AppBuffer()
{
    delete[] tempBuf;
}

int AppBuffer::bytesRemaining()
{
    return (bufferLength - readOffset);
}

UINT8 AppBuffer::getUINT8()
{
    return (tempBuf[readOffset++]);
}

UINT16 AppBuffer::getUINT16()
{
    UINT8* tempBufPtr = &tempBuf[readOffset];
    UINT16 v = (isLittleEndian == 0) ? *(UINT16*)tempBufPtr : (tempBufPtr[1] << 0) | (tempBufPtr[0] << 8);
    readOffset += 2;
    return (v);
}

UINT32 AppBuffer::getUINT32()
{
    UINT8* tempBufPtr = &tempBuf[readOffset];
    UINT32 v = (isLittleEndian == 0) ? *(UINT32*)tempBufPtr : (tempBufPtr[3] << 0) | (tempBufPtr[2] << 8) | (tempBufPtr[1] << 16) | (tempBufPtr[0] << 24);
    readOffset += 4;
    return (v);
}

int AppBuffer::allocate(int bytes)
{
    int loc = writeOffset;
    writeOffset += bytes;
    return (loc);
}

void AppBuffer::setUINT8(UINT8 v)
{
    setUINT8(writeOffset++, v);
}

void AppBuffer::setUINT16(UINT16 v)
{
    setUINT16(writeOffset, v);
    writeOffset += 2;
}

void AppBuffer::setUINT32(UINT32 v)
{
    setUINT32(writeOffset, v);
    writeOffset += 4;
}

void AppBuffer::setUINT8(int byteOffset, UINT8 v)
{
    tempBuf[byteOffset] = v;
}

void AppBuffer::setUINT16(int byteOffset, UINT16 v)
{
    UINT8* tempBufPtr = &tempBuf[byteOffset];
    if (isLittleEndian == 1)
    {
        UINT8* vp = (UINT8*)&v;
        v = (vp[1] << 0) | (vp[0] << 8);
    }

    *((UINT16*)tempBufPtr) = v;
}

void AppBuffer::setUINT32(int byteOffset, UINT32 v)
{
    UINT8* tempBufPtr = &tempBuf[byteOffset];
    if (isLittleEndian == 1)
    {
        UINT8* vp = (UINT8*)&v;
        v = (vp[3] << 0) | (vp[2] << 8) | (vp[1] << 16) | (vp[0] << 24);
    }

    *((UINT32*)tempBufPtr) = v;
}
