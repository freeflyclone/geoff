#include "AppBuffer.h"

AppBuffer::AppBuffer(std::size_t bufferLength, int isLittleEndian)
    :
    m_isLittleEndian(isLittleEndian),
    bufferLength(bufferLength)
{
    tempBuf = new uint8_t[bufferLength];
    writeOffset = 0;
}

AppBuffer::~AppBuffer()
{
    delete[] tempBuf;
}

void AppBuffer::setLittleEndian(bool isLittleEndian)
{
    m_isLittleEndian = isLittleEndian;
}

std::size_t AppBuffer::bytesRemaining()
{
    return (bufferLength - readOffset);
}

uint8_t AppBuffer::get_uint8()
{
    return (tempBuf[readOffset++]);
}

uint16_t AppBuffer::get_uint16()
{
    uint8_t* tempBufPtr = &tempBuf[readOffset];
    uint16_t v = !m_isLittleEndian ? *(uint16_t*)tempBufPtr : (tempBufPtr[1] << 0) | (tempBufPtr[0] << 8);
    readOffset += 2;
    return (v);
}

uint32_t AppBuffer::get_uint32()
{
    uint8_t* tempBufPtr = &tempBuf[readOffset];
    uint32_t v = !m_isLittleEndian ? *(uint32_t*)tempBufPtr : (tempBufPtr[3] << 0) | (tempBufPtr[2] << 8) | (tempBufPtr[1] << 16) | (tempBufPtr[0] << 24);
    readOffset += 4;
    return (v);
}

int AppBuffer::allocate(int bytes)
{
    int loc = writeOffset;
    writeOffset += bytes;
    return (loc);
}

void AppBuffer::set_uint8(uint8_t v)
{
    set_uint8(writeOffset++, v);
}

void AppBuffer::set_uint16(uint16_t v)
{
    set_uint16(writeOffset, v);
    writeOffset += 2;
}

void AppBuffer::set_uint32(uint32_t v)
{
    set_uint32(writeOffset, v);
    writeOffset += 4;
}

void AppBuffer::set_uint8(int byteOffset, uint8_t v)
{
    tempBuf[byteOffset] = v;
}

void AppBuffer::set_uint16(int byteOffset, uint16_t v)
{
    uint8_t* tempBufPtr = &tempBuf[byteOffset];
    if (m_isLittleEndian)
    {
        uint8_t* vp = (uint8_t*)&v;
        v = (vp[1] << 0) | (vp[0] << 8);
    }

    *((uint16_t*)tempBufPtr) = v;
}

void AppBuffer::set_uint32(int byteOffset, uint32_t v)
{
    uint8_t* tempBufPtr = &tempBuf[byteOffset];
    if (m_isLittleEndian)
    {
        uint8_t* vp = (uint8_t*)&v;
        v = (vp[3] << 0) | (vp[2] << 8) | (vp[1] << 16) | (vp[0] << 24);
    }

    *((uint32_t*)tempBufPtr) = v;
}
