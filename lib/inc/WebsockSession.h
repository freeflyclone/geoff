/*
* WebsockSession
* 
* This class represents an interface between boost::beast code (websocket_session.h) 
* and GEOFF C++ classes. It provides minimum services to send AppBuffer packets between
* browser-based JavaScript client code and our C++ classes via a WebSocket interface;
* 
* Special thanks to Vinnie Falco for inspiration.
* 
* Services provided:
*   Managment of AppBuffer marshalling to and from beast::flat_buffer
*   Client-side endianness at runtime
*   Unique Session ID at runtime
*   std::deque management of AppBuffers using std::unique_ptr
*		Automatically manage AppBuffers
*   Handling of incoming AppBuffers from client side via CommsHandler
*   Handling of outbound AppBuffers to client side via CommitTxBuffer
* 
*   See also GameSession class.
* 
*	Changing this header WILL cause websocket_session.h to be reloaded/parsed.
*/
#ifndef WEBSOCK_SESSION_H
#define WEBSOCK_SESSION_H

#include <list>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>

#include "AppBuffer.h"

class WebsockSession
{
public:
	// used to initiate beast::async_write() when new TX buffer is ready
	// that isn't a response to a query from the client side.
	typedef std::function<void(WebsockSession &)> OnTxReadyCallback_t;

	enum class RequestType_t
	{
		RegisterSession = 0x00,
		ClickEvent = 0x02,
		KeyEvent = 0x04,
		ResizeEvent = 0x06,
	};

	enum class MessageType_t
	{
		SessionRegistered = 0x01,
		ClickMessage = 0x03,
		KeyMessage = 0x05,
		PlayerTickMessage = 0x07,
		UniverseTickMessage = 0x09
	};

	WebsockSession(uint32_t sessionID);
	virtual ~WebsockSession();

	// Allow caller to specify a callback when a new AppBuffer for TX is ready
	// (touching websocket_session is expensive to compile, this callback helps minimize
	// dependencies)
	void SetOnTxReadyCallback(OnTxReadyCallback_t);

	uint32_t SessionID();
	bool IsLittleEndian() { return m_isLittleEndian; }

	// Called directly from within websocket_session::on_read() via WebsockServer::CommsHandler
	void CommsHandler(beast::flat_buffer buffer, std::size_t bytes_transferred);

	// Called by any routine that wants to transmit an AppBuffer to the client side
	void CommitTxBuffer(std::unique_ptr<AppBuffer>& buffer);

	// Called in from websocket_session::on_write() and other places, to check for and return
	// the next AppBuffer to send to the client side.
	bool TxQueueEmpty() { return m_txQue.empty(); }
	bool GetNextTxBuffer(std::unique_ptr<AppBuffer>& buffer);

	virtual void CommsHandler(const uint8_t* buff, const size_t length);
	virtual void CommsHandler(AppBuffer& buffer);

	// Call the TX ready callback specified by the caller
	void OnTxReady(WebsockSession&);

protected:
	uint32_t m_sessionID;
	bool m_isLittleEndian;

	std::deque<std::unique_ptr<AppBuffer>> m_txQue;
	std::recursive_mutex m_session_mutex;

	OnTxReadyCallback_t m_tx_ready_callback;
	std::thread::id m_thread_id;
};

#endif // WEBSOCK_SESSION_H
