#include "geoff.h"
#include "WebsockSession.h"
#include "WebsockServer.h"

WebsockSession::WebsockSession(uint32_t sessionID) :
	m_sessionID(sessionID),
	m_thread_done(false),
	m_tick_count(0)
{
}

WebsockSession::~WebsockSession()
{
	m_thread_done = true;
	if (m_thread.joinable())
		m_thread.join();
}

uint32_t WebsockSession::SessionID()
{
	return m_sessionID;
}

void WebsockSession::CommsHandler(const uint8_t* buff, const size_t length)
{
	if (!buff || length < 2)
		return;

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
	}
}

void WebsockSession::RegisterNewSession(AppBuffer& rxBuffer)
{
	// skip "clientAppVersion" for now.
	rxBuffer.get_uint16();

	auto txBuffer = std::make_unique<AppBuffer>(12, m_isLittleEndian);

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x01);
	txBuffer->set_uint32(m_sessionID);
	txBuffer->set_uint16((uint16_t)gameAppVersion);

	WebsockServer::GetInstance().CommitTxBuffer(txBuffer);

	m_thread = std::thread(std::bind(&WebsockSession::TimerTick, this));
}

void WebsockSession::HandleKeyEvent(AppBuffer& rxBuffer)
{
	bool isDown = (rxBuffer.get_uint8() == 1) ? true : false;
	int keyCode = rxBuffer.get_uint8();

	auto txBuffer = std::make_unique <AppBuffer>(8, m_isLittleEndian);

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x05);
	txBuffer->set_uint32(m_sessionID);
	txBuffer->set_uint8(isDown ? 1 : 0);
	txBuffer->set_uint8(static_cast<uint8_t>(keyCode));

	WebsockServer::GetInstance().CommitTxBuffer(txBuffer);
}

void WebsockSession::HandleClickEvent(AppBuffer& rxBuffer)
{
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

	WebsockServer::GetInstance().CommitTxBuffer(txBuffer);
}

void WebsockSession::TimerTick()
{
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds

	while (!m_thread_done)
	{
		sleep_until(system_clock::now() + milliseconds(16));

		auto txBuffer = std::make_unique <AppBuffer>(12, m_isLittleEndian);

		txBuffer->set_uint8(0xBB);
		txBuffer->set_uint8(0x03);
		txBuffer->set_uint32(m_sessionID);
		txBuffer->set_uint32(m_tick_count++);

		WebsockServer::GetInstance().CommitTxBuffer(txBuffer);
		WebsockServer::GetInstance().OnTxReady();
	}
}

WebsockSessionManager::WebsockSessionManager()
	: 
	m_session_id(0),
	m_sessions()
{
}

WebsockSessionManager::~WebsockSessionManager()
{
}

uint32_t WebsockSessionManager::add_session()
{
	const std::lock_guard<std::mutex> lock(m_sessions_mutex);

	auto session_id = m_session_id;

	m_sessions.push_back(std::make_shared<WebsockSession>(session_id));

	m_session_id = (m_session_id + 1) & 0xFFFFFFFF;

	return session_id;
}

void WebsockSessionManager::delete_by_id(uint32_t sessionID)
{
	const std::lock_guard<std::mutex> lock(m_sessions_mutex);

	auto session = find_by_id(sessionID);

	if (session)
		m_sessions.remove(session);
}

std::shared_ptr<WebsockSession> WebsockSessionManager::find_by_id(uint32_t sessionID)
{
	for (auto session : m_sessions)
	{
		if (session->SessionID() == sessionID)
			return session;
	}

	return nullptr;
}
