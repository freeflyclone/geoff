#ifndef GEOFF_H
#define GEOFF_H

#define _USE_MATH_DEFINES
#include <cmath>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/make_unique.hpp>
#include <boost/optional.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <deque>

// namespace aliases - less typing in source code
namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace http = beast::http;                   // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;         // from <boost/beast/websocket.hpp>
namespace net = boost::asio;                    // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;               // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

#define GEOFF_Version "1.0"
#define TRACE(...) {std::cerr << __FILE__ << ":" << __LINE__ << ", " << __VA_ARGS__ << std::endl;}

int Geoff(std::string addr, std::string prt, std::string www_root, std::string thrds);

#endif