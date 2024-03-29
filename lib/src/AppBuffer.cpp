#include <cstring>
#include <cstdint>

#include "AppBuffer.h"
#include "geoff.h"

#define AB_TRACE(...)
//#define AB_TRACE TRACE

AppBuffer::AppBuffer(std::size_t bufferLength, int isLittleEndian)
    :
    m_length(bufferLength),
    m_isLittleEndian(isLittleEndian),
    m_readOffset(0),
    m_writeOffset(0)
{
    m_buff = new uint8_t[bufferLength];
    memset(m_buff, 0, m_length);
}

AppBuffer::AppBuffer(const uint8_t *buffer, const std::size_t bufferLength, int isLittleEndian)
    :
    AppBuffer(bufferLength, isLittleEndian)
{
    std::memcpy(m_buff, buffer, m_length);
}

AppBuffer::AppBuffer(AppBuffer& ab, size_t moreRoom, bool isLittleEndian)
    :
    AppBuffer(ab.bytesWritten() + moreRoom, isLittleEndian)
{
    memcpy(m_buff, ab.data(), ab.bytesWritten());
    m_writeOffset = static_cast<int>(ab.bytesWritten());

    AB_TRACE(__FUNCTION__ << "bytesWritten(): " << ab.bytesWritten() << ", moreRoom: " << moreRoom << "m_length: " << m_length);
}

AppBuffer::AppBuffer(AppBuffer& ab1, AppBuffer& ab2, bool isLittleEndian)
    :
    AppBuffer(ab1.bytesWritten() + ab2.bytesWritten(), isLittleEndian)
{
    memcpy(m_buff, ab1.data(), ab1.bytesWritten());
    memcpy(m_buff + ab1.bytesWritten(), ab2.data(), ab2.bytesWritten());
    m_writeOffset = static_cast<int>(ab1.bytesWritten() + ab2.bytesWritten());

    AB_TRACE(__FUNCTION__ << "bytesWritten(): " << ab.bytesWritten() << ", moreRoom: " << moreRoom << "m_length: " << m_length);
}

AppBuffer::~AppBuffer()
{
    delete[] m_buff;
}

std::size_t AppBuffer::bytesRemaining()
{
    return (m_length - m_readOffset);
}

std::size_t AppBuffer::bytesWritten()
{
    return m_writeOffset;
}

std::size_t AppBuffer::bytesRead()
{
    return m_readOffset;
}

std::size_t AppBuffer::size()
{
    return m_length;
}

uint8_t* AppBuffer::data()
{
    assert(m_buff);
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

uint8_t AppBuffer::get_uint8(int byteOffset)
{
    return (m_buff[byteOffset]);
}

uint16_t AppBuffer::get_uint16(int byteOffset)
{
    uint8_t* tempBufPtr = &m_buff[byteOffset];
    uint16_t v = !m_isLittleEndian ? *(uint16_t*)tempBufPtr : (tempBufPtr[1] << 0) | (tempBufPtr[0] << 8);
    return (v);
}

uint32_t AppBuffer::get_uint32(int byteOffset)
{
    uint8_t* tempBufPtr = &m_buff[byteOffset];
    uint32_t v = !m_isLittleEndian ? *(uint32_t*)tempBufPtr : (tempBufPtr[3] << 0) | (tempBufPtr[2] << 8) | (tempBufPtr[1] << 16) | (tempBufPtr[0] << 24);
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
