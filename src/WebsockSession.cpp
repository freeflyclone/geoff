#include "geoff.h"
#include "WebsockSession.h"
#include "WebsockServer.h"

#define INTERVAL_IN_MS 500

WebsockSession::WebsockSession(uint32_t sessionID) :
	m_sessionID(sessionID),
	m_isLittleEndian(true),
	m_run_timer(false),
	m_timer_complete(false),
	m_timer_tick(0)
{
	TRACE(std::endl);
	TRACE("sessionID: " << m_sessionID);
	
	m_timer = std::make_unique<net::deadline_timer>(*WebsockServer::GetInstance().IoContext(), boost::posix_time::milliseconds(INTERVAL_IN_MS));
}

WebsockSession::~WebsockSession()
{
	TRACE("sessionID: " << m_sessionID << std::endl);

	m_run_timer = false;
	m_timer_complete = 5;
	while (m_timer_complete >= 0)
	{
		m_run_timer = false;
		std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(10)));
		m_timer_complete--;
	}
}

uint32_t WebsockSession::SessionID()
{
	return m_sessionID;
}

void WebsockSession::CommsHandler(const uint8_t* buff, const size_t length)
{
	if (!buff || length < 2)
		return;

	TRACE(std::endl);
	TRACE("Begin" << ", sessionID: " << m_sessionID);

	// 1st 2 packet header bytes: AA (bigendian) or AB (littleendian), followed by RequestType_t
	m_isLittleEndian = buff[0] == 0xAB ? true : false;
	auto requestType = static_cast<WebsockSession::RequestType_t>(buff[1]);

	AppBuffer rxBuffer(buff+2, length-2, m_isLittleEndian);

	switch (requestType)
	{
		case RequestType_t::RegisterSession:
			RegisterNewSession(rxBuffer);
			break;

		case RequestType_t::KeyEvent:
			HandleKeyEvent(rxBuffer);
			break;

		case RequestType_t::ClickEvent:
			HandleClickEvent(rxBuffer);
			break;

		default:
			TRACE("default invoked");
			break;
	}

	TRACE("End" << ", sessionID: " << m_sessionID << std::endl);
}

void WebsockSession::RegisterNewSession(AppBuffer& rxBuffer)
{
	TRACE(", sessionID: " << m_sessionID);

	// skip "clientAppVersion" for now.
	rxBuffer.get_uint16();

	auto txBuffer = std::make_unique<AppBuffer>(12, m_isLittleEndian);

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x01);
	txBuffer->set_uint32(m_sessionID);
	txBuffer->set_uint16((uint16_t)gameAppVersion);

	CommitTxBuffer(txBuffer);

	m_run_timer = true;
	TimerTick();
}

void WebsockSession::HandleKeyEvent(AppBuffer& rxBuffer)
{
	TRACE(", sessionID: " << m_sessionID);

	bool isDown = (rxBuffer.get_uint8() == 1) ? true : false;
	int keyCode = rxBuffer.get_uint8();

	auto txBuffer = std::make_unique <AppBuffer>(8, m_isLittleEndian);

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x05);
	txBuffer->set_uint32(m_sessionID);
	txBuffer->set_uint8(isDown ? 1 : 0);
	txBuffer->set_uint8(static_cast<uint8_t>(keyCode));

	CommitTxBuffer(txBuffer);
}

void WebsockSession::HandleClickEvent(AppBuffer& rxBuffer)
{
	TRACE(", sessionID: " << m_sessionID);

	uint32_t rxSessionID = rxBuffer.get_uint32();
	uint16_t playerID = rxBuffer.get_uint16();
	uint16_t clickX = rxBuffer.get_uint16();
	uint16_t clickY = rxBuffer.get_uint16();

	auto txBuffer = std::make_unique <AppBuffer>(12, m_isLittleEndian);

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x03);
	txBuffer->set_uint32(m_sessionID);
	txBuffer->set_uint16(playerID);
	txBuffer->set_uint16(clickX);
	txBuffer->set_uint16(clickY);

	CommitTxBuffer(txBuffer);
}

void WebsockSession::CommsHandler(beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	TRACE(", sessionID: " << m_sessionID);

	uint8_t* buff = (uint8_t*)static_cast<net::const_buffer>(in_buffer.data()).data();

	CommsHandler(buff, in_length);
}

void WebsockSession::CommitTxBuffer(std::unique_ptr<AppBuffer>& buffer)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	TRACE(", sessionID: " << m_sessionID);

	m_txQue.push_back(std::move(buffer));
}

bool WebsockSession::GetNextTxBuffer(std::unique_ptr<AppBuffer>& buff)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	TRACE(", sessionID: " << m_sessionID);

	if (m_txQue.empty())
		return false;

	// Get next AppBuffer from TX que
	buff = std::move(m_txQue.front());
	m_txQue.pop_front();

	return true;
}

void WebsockSession::TimerTick()
{
	TRACE("Timer fired, session id: " << m_sessionID);

	if (!m_timer)
	{
		TRACE("")
		m_timer_complete = true;
		return;
	}

	if (!m_run_timer)
	{
		TRACE("")
		m_timer_complete = true;
		return;
	}

	auto txBuff = std::make_unique<AppBuffer>(12, m_isLittleEndian);

	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(0x07);
	txBuff->set_uint32(m_sessionID);
	txBuff->set_uint32(m_timer_tick++);

	this->CommitTxBuffer(txBuff);

	// TODO figure out where our data is going.
	WebsockServer::GetInstance().OnTxReady(m_sessionID);

	boost::system::error_code ec;
	m_timer->expires_from_now(boost::posix_time::milliseconds(INTERVAL_IN_MS), ec);
	if (ec)
	{
		TRACE(ec);
		return;
	}

	m_timer->async_wait([this](const boost::system::error_code& e) {
		if (e)
		{
			TRACE(e);
			return;
		}
		this->TimerTick();
	});
}

WebsockSessionManager::WebsockSessionManager()
	: 
	m_session_id(0),
	m_sessions()
{
	TRACE("");
}

WebsockSessionManager::~WebsockSessionManager()
{
	TRACE("");
}

uint32_t WebsockSessionManager::add_session()
{
	const std::lock_guard<std::mutex> lock(m_sessions_mutex);
	TRACE("");

	auto session_id = m_session_id;

	m_sessions.push_back(std::make_shared<WebsockSession>(session_id));

	m_session_id = (m_session_id + 1) & 0xFFFFFFFF;

	return session_id;
}

void WebsockSessionManager::delete_by_id(uint32_t sessionID)
{
	const std::lock_guard<std::mutex> lock(m_sessions_mutex);
	TRACE("");

	auto session = find_by_id(sessionID);

	if (session)
	{
		TRACE("session id: " << sessionID);
		m_sessions.remove(session);
	}
}

std::shared_ptr<WebsockSession> WebsockSessionManager::find_by_id(uint32_t sessionID)
{
	for (auto session : m_sessions)
	{
		if (session->SessionID() == sessionID)
		{
			TRACE("session id: " << sessionID);
			return session;
		}
	}

	return nullptr;
}
