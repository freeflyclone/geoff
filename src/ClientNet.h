#pragma once

#ifndef ClientNet_H
#define ClientNet_H

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

class ClientNet
{
public:

    //websocket::stream<beast::tcp_stream>* ws_;
    int isLittleEndian;
    int framesWithoutContact;

    ClientNet();
    void initialise(int isLittleEndian);
};

class AppBuffer
{

public:
    UINT8* tempBuf;
    UINT8  isLittleEndian;

    int    bufferLength;

    int    readOffset = 0;
    int    writeOffset = 0;

    AppBuffer(int bufferLength, int isLittleEndian);
    ~AppBuffer();

    int bytesRemaining();

    UINT8 getUINT8();
    UINT16 getUINT16();
    UINT32 getUINT32();

    int allocate(int bytes);

    void setUINT8(UINT8 v);
    void setUINT16(UINT16 v);
    void setUINT32(UINT32 v);

    void setUINT8(int byteOffset, UINT8 v);
    void setUINT16(int byteOffset, UINT16 v);
    void setUINT32(int byteOffset, UINT32 v);
};

#endif
