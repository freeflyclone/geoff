#ifndef GAME_H
#define GAME_H

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

#define gameAppVersion 2

class Game
{
public:
	enum RequestType_t
	{
		RegisterClient = 0x08,
		KeyEvent = 0x12
	};
	static Game& GetInstance();

	void CommsHandler(beast::flat_buffer buffer, std::size_t bytes_transferred);
	bool GetNextTxBuffer(std::shared_ptr<AppBuffer>& buffer);

private:
	// I know what you're thinking: WTF is this?  It's Magic Statics!
	// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
	// 
	// I tried to figure out how to pass a Game object into the Beast code, and fully face-planted.
	// So I said fuckit, I'm using a Singleton.
	Game();
	~Game();

	// You can only get a Game instance through Game::GetInstance(), all default constructors hereby deleted.
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;

	void RegisterNewClientConnection(AppBuffer & rxBuffer);
	void HandleKeyEvent(AppBuffer & rxBuffer);

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

	uint16_t m_clientID;
	uint16_t m_mapWidth, m_mapHeight;
};

#endif