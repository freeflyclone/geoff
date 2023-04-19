#pragma once


#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class AppBuffer
{
public:
    AppBuffer(std::size_t bufferLength, int isLittleEndian);
    AppBuffer(uint8_t* buffer, std::size_t bufferLength, int isLittleEndian);
    ~AppBuffer();

    std::size_t bytesRemaining();

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

private:
    uint8_t* m_buff;
    std::size_t    m_length;
    bool  m_isLittleEndian;

    int    m_readOffset = 0;
    int    m_writeOffset = 0;
};
