#include "geoff.h"
#include "WebsockSession.h"
#include "WebsockServer.h"
#include "GameSession.h"

//#define WS_TRACE TRACE
#define WS_TRACE(...)

WebsockSession::WebsockSession(uint32_t sessionID) :
	m_sessionID(sessionID),
	m_isLittleEndian(true),
	m_tx_ready_callback()
{
	WS_TRACE("sessionID: " << m_sessionID);
}

WebsockSession::~WebsockSession()
{
	WS_TRACE("sessionID: " << m_sessionID);
}

void WebsockSession::SetOnTxReadyCallback(OnTxReadyCallback_t fn)
{
	m_tx_ready_callback = fn;
}

void WebsockSession::OnTxReady(WebsockSession &self)
{
	if (m_tx_ready_callback)
		m_tx_ready_callback(self);
}

uint32_t WebsockSession::SessionID()
{
	return m_sessionID;
}

void WebsockSession::CommsHandler(const uint8_t* buff, const size_t length)
{
	if (!buff || length < 2)
		return;

	// 1st packet header byte: AA (bigendian) or AB (littleendian)
	m_isLittleEndian = buff[0] == 0xAB ? true : false;

	WS_TRACE("Creating AppBuffer");

	AppBuffer rxBuffer(buff+1, length-1, m_isLittleEndian);

	CommsHandler(rxBuffer);
}

void WebsockSession::CommsHandler(AppBuffer& buffer)
{
	(void)buffer;
	WS_TRACE("Dummy CommsHandler, should be overridden by derived classes");
}

void WebsockSession::CommsHandler(beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	WS_TRACE("Receive beast::flat_buffer, sessionID: " << m_sessionID);

	uint8_t* buff = (uint8_t*)static_cast<net::const_buffer>(in_buffer.data()).data();

	CommsHandler(buff, in_length);
}

void WebsockSession::CommitTxBuffer(std::unique_ptr<AppBuffer>& buffer)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	WS_TRACE("unique_ptr to txQue, sessionID: " << m_sessionID);

	m_txQue.push_back(std::move(buffer));
}

bool WebsockSession::GetNextTxBuffer(std::unique_ptr<AppBuffer>& buff)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	if (m_txQue.empty())
		return false;

	WS_TRACE("pop next unique_ptr from txQue, sessionID: " << m_sessionID);

	// Get next AppBuffer from TX que
	buff = std::move(m_txQue.front());
	m_txQue.pop_front();

	return true;
}
