#include <cstring>
#include <cstdint>

#include "AppBuffer.h"

AppBuffer::AppBuffer(std::size_t bufferLength, int isLittleEndian)
    :
    m_length(bufferLength),
    m_isLittleEndian(isLittleEndian),
    m_readOffset(0),
    m_writeOffset(0)
{
    m_buff = new uint8_t[bufferLength];
}

AppBuffer::AppBuffer(uint8_t *buffer, std::size_t bufferLength, int isLittleEndian)
    :
    AppBuffer(bufferLength, isLittleEndian)
{
    std::memcpy(m_buff, buffer, m_length);
}

AppBuffer::~AppBuffer()
{
    delete[] m_buff;
}

void AppBuffer::setLittleEndian(bool isLittleEndian)
{
    m_isLittleEndian = isLittleEndian;
}

std::size_t AppBuffer::bytesRemaining()
{
    return (m_length - m_readOffset);
}

std::size_t AppBuffer::bytesWritten()
{
    return (m_writeOffset);
}

uint8_t* AppBuffer::data()
{
    return m_buff;
}

uint8_t AppBuffer::get_uint8()
{
    return (m_buff[m_readOffset++]);
}

uint16_t AppBuffer::get_uint16()
{
    uint8_t* tempBufPtr = &m_buff[m_readOffset];
    uint16_t v = !m_isLittleEndian ? *(uint16_t*)tempBufPtr : (tempBufPtr[1] << 0) | (tempBufPtr[0] << 8);
    m_readOffset += 2;
    return (v);
}

uint32_t AppBuffer::get_uint32()
{
    uint8_t* tempBufPtr = &m_buff[m_readOffset];
    uint32_t v = !m_isLittleEndian ? *(uint32_t*)tempBufPtr : (tempBufPtr[3] << 0) | (tempBufPtr[2] << 8) | (tempBufPtr[1] << 16) | (tempBufPtr[0] << 24);
    m_readOffset += 4;
    return (v);
}

int AppBuffer::allocate(int bytes)
{
    int loc = m_writeOffset;
    m_writeOffset += bytes;
    return (loc);
}

void AppBuffer::set_uint8(uint8_t v)
{
    set_uint8(m_writeOffset++, v);
}

void AppBuffer::set_uint16(uint16_t v)
{
    set_uint16(m_writeOffset, v);
    m_writeOffset += 2;
}

void AppBuffer::set_uint32(uint32_t v)
{
    set_uint32(m_writeOffset, v);
    m_writeOffset += 4;
}

void AppBuffer::set_uint8(int byteOffset, uint8_t v)
{
    m_buff[byteOffset] = v;
}

void AppBuffer::set_uint16(int byteOffset, uint16_t v)
{
    uint8_t* tempBufPtr = &m_buff[byteOffset];
    if (m_isLittleEndian)
    {
        uint8_t* vp = (uint8_t*)&v;
        v = (vp[1] << 0) | (vp[0] << 8);
    }

    *((uint16_t*)tempBufPtr) = v;
}

void AppBuffer::set_uint32(int byteOffset, uint32_t v)
{
    uint8_t* tempBufPtr = &m_buff[byteOffset];
    if (m_isLittleEndian)
    {
        uint8_t* vp = (uint8_t*)&v;
        v = (vp[3] << 0) | (vp[2] << 8) | (vp[1] << 16) | (vp[0] << 24);
    }

    *((uint32_t*)tempBufPtr) = v;
}
