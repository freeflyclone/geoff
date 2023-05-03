#ifndef WEBSOCK_SERVER_H
#define WEBSOCK_SERVER_H

/*
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
*/

#include "geoff.h"
#include "AppBuffer.h"
#include "WebsockSession.h"

#define gameAppVersion 2

class WebsockServer
{
public:
	static WebsockServer& GetInstance();
	typedef std::function<void(uint32_t sessionId)> OnAcceptCallback_t;
	typedef std::function<void(uint32_t sessionID)> OnTxReady_t;

	void IoContext(net::io_context* ioc);
	net::io_context* IoContext();

	void OnAccept(OnAcceptCallback_t);
	void OnTxReady(OnTxReady_t);
	void OnTxReady(uint32_t sessionID);
	void OnClose(uint32_t sessionID);

	void CommsHandler(uint32_t sessionID, beast::flat_buffer buffer, std::size_t bytes_transferred);
	std::shared_ptr<WebsockSession> FindSessionByID(uint32_t sessionID);
	/*
	void CommitTxBuffer(std::unique_ptr<AppBuffer>& buffer);
	bool GetNextTxBuffer(std::unique_ptr<AppBuffer>& buffer);
	*/

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

	std::recursive_mutex m_serverMutex;

	WebsockSessionManager m_sessions;
	OnTxReady_t m_on_tx_ready_cb;
	net::io_context *m_ioc;
};

#endif