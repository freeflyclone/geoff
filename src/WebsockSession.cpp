#include "geoff.h"
#include "WebsockSession.h"
#include "WebsockServer.h"

WebsockSession::WebsockSession(uint32_t sessionID) :
	m_sessionID(sessionID),
	m_isLittleEndian(true)
{
}

WebsockSession::~WebsockSession()
{
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

	CommitTxBuffer(txBuffer);
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

	CommitTxBuffer(txBuffer);
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

	CommitTxBuffer(txBuffer);
}

void WebsockSession::CommsHandler(beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	uint8_t* buff = (uint8_t*)static_cast<net::const_buffer>(in_buffer.data()).data();

	CommsHandler(buff, in_length);
}

void WebsockSession::CommitTxBuffer(std::unique_ptr<AppBuffer>& buffer)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	m_txQue.push_back(std::move(buffer));
}

bool WebsockSession::GetNextTxBuffer(std::unique_ptr<AppBuffer>& buff)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	if (m_txQue.empty())
		return false;

	// Get next AppBuffer from TX que
	buff = std::move(m_txQue.front());
	m_txQue.pop_front();

	return true;
}

WebsockSessionManager::WebsockSessionManager()
	: 
	m_session_id(0),
	m_sessions(),
	m_thread_running(false),
	m_tick_count(0)
{
}

WebsockSessionManager::~WebsockSessionManager()
{
	m_thread_running = false;

	if (m_thread.joinable())
		m_thread.join();
}

uint32_t WebsockSessionManager::add_session()
{
	const std::lock_guard<std::mutex> lock(m_sessions_mutex);

	auto session_id = m_session_id;

	m_sessions.push_back(std::make_shared<WebsockSession>(session_id));

	m_session_id = (m_session_id + 1) & 0xFFFFFFFF;


	if (!m_thread_running)
	{
		m_thread_running = true;
		m_thread = std::thread(std::bind(&WebsockSessionManager::TimerTick, this));
	}

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

void WebsockSessionManager::TimerTick()
{
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds

	while (m_thread_running)
	{
		TRACE("");
		for (auto session : m_sessions)
		{
			net::post(*(WebsockServer::GetInstance().IoContext()), [this, session]() {
				auto txBuffer = std::make_unique <AppBuffer>(12, session->IsLittleEndian());

				txBuffer->set_uint8(0xBB);
				txBuffer->set_uint8(0x07);
				txBuffer->set_uint32(session->SessionID());
				txBuffer->set_uint32(m_tick_count);

				session->CommitTxBuffer(txBuffer);
				WebsockServer::GetInstance().OnTxReady(session->SessionID());
			});
		}
		m_tick_count++;
		sleep_until(system_clock::now() + milliseconds(500));
	}
}

