#pragma once

#include <cstdlib>

class AppBuffer
{
public:
    AppBuffer(std::size_t bufferLength, int isLittleEndian);
    AppBuffer(uint8_t* buffer, std::size_t bufferLength, int isLittleEndian);
    ~AppBuffer();

    size_t bytesRemaining();
    size_t bytesWritten();
    uint8_t* data();

    uint8_t get_uint8();
    uint16_t get_uint16();
    uint32_t get_uint32();

    int allocate(int bytes);

    void set_uint8(uint8_t v);
    void set_uint16(uint16_t v);
    void set_uint32(uint32_t v);

    void set_uint8(int byteOffset, uint8_t v);
    void set_uint16(int byteOffset, uint16_t v);
    void set_uint32(int byteOffset, uint32_t v);

    void setLittleEndian(bool isLittleEndian);
    bool isLittleEndian() { return m_isLittleEndian; }

private:
    uint8_t* m_buff;
    std::size_t    m_length;
    bool  m_isLittleEndian;

    int    m_readOffset = 0;
    int    m_writeOffset = 0;
};
