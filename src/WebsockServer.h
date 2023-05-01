#ifndef WEBSOCK_SERVER_H
#define WEBSOCK_SERVER_H

#include <deque>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#include "AppBuffer.h"
#include "WebsockSession.h"

#define gameAppVersion 2

class WebsockServer
{
public:
	enum RequestType_t
	{
		RegisterSession = 0x08,
		ClickEvent = 0x12,
		KeyEvent = 0x14
	};
	static WebsockServer& GetInstance();
	typedef std::function<void(uint32_t sessionId)> OnAcceptCallback_t;

	void OnAccept(OnAcceptCallback_t);
	void OnClose(uint32_t sessionID);

	void CommsHandler(uint32_t sessionID, beast::flat_buffer buffer, std::size_t bytes_transferred);
	bool GetNextTxBuffer(std::shared_ptr<AppBuffer>& buffer);

private:
	// I know what you're thinking: WTF is this?  It's Magic Statics!
	// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
	// 
	// I tried to figure out how to pass a WebsockServer object into the Beast code, and fully face-planted.
	// So I said fuckit, I'm using a Singleton.
	WebsockServer();
	~WebsockServer();

	// You can only get a WebsockServer instance through WebsockServer::GetInstance(), all default constructors hereby deleted.
	WebsockServer(const WebsockServer&) = delete;
	WebsockServer& operator=(const WebsockServer&) = delete;
	WebsockServer(WebsockServer&&) = delete;
	WebsockServer& operator=(WebsockServer&&) = delete;

	void RegisterNewSession(uint32_t sessionID, AppBuffer & rxBuffer);
	void HandleClickEvent(uint32_t sessionID, AppBuffer& rxBuffer);
	void HandleKeyEvent(uint32_t sessionID, AppBuffer & rxBuffer);

	// std::deque *might* be overkill, std::queue would probably suffice here.
	// 
	// Note on shared_ptr usage: GetNextTxBuffer() empties this que into a shared_ptr
	// reference provided by it's caller.  When the caller's shared_ptr reference
	// goes out of scope, the underlying AppBuffer is then deconstructed. (freed)
	// 
	// See websocket_session.h on_read() function, where the underlying data is copied into
	// a boost::asio::buffer() object for the lifetime of the async_write() operation.
	std::deque<std::shared_ptr<AppBuffer>> m_txQue;

	std::mutex m_playersMutex;

	uint32_t m_sessionID;
	uint16_t m_mapWidth, m_mapHeight;

	WebsockSessionManager m_sessions;
};

#endif