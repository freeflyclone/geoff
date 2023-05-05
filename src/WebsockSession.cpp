#include "geoff.h"
#include "WebsockSession.h"
#include "WebsockServer.h"
#include "GameSession.h"

WebsockSession::WebsockSession(uint32_t sessionID) :
	m_sessionID(sessionID),
	m_isLittleEndian(true),
	m_run_timer(false),
	m_timer_complete(false),
	m_timer_tick(0),
	m_tx_ready_callback(),
	m_tick_interval_in_us(500000)
{
	//TRACE("sessionID: " << m_sessionID);
	
	m_timer = std::make_unique<net::deadline_timer>(*WebsockServer::GetInstance().IoContext(), boost::posix_time::microseconds(m_tick_interval_in_us));
}

WebsockSession::~WebsockSession()
{
	//TRACE("sessionID: " << m_sessionID);

	m_run_timer = false;
	m_timer_complete = 5;
	while (m_timer_complete >= 0)
	{
		m_run_timer = false;
		std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(10)));
		m_timer_complete--;
	}
}

void WebsockSession::OnTxReady(OnTxReadyCallback_t fn)
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

	// 1st 2 packet header bytes: AA (bigendian) or AB (littleendian), followed by RequestType_t
	m_isLittleEndian = buff[0] == 0xAB ? true : false;
	auto requestType = static_cast<WebsockSession::RequestType_t>(buff[1]);

	AppBuffer rxBuffer(buff+1, length-1, m_isLittleEndian);

	CommsHandler(rxBuffer);
}

void WebsockSession::CommsHandler(AppBuffer& buffer)
{
	(void)buffer;
}

void WebsockSession::StartTimer()
{
	m_run_timer = true;
	TimerTick();
}

void WebsockSession::StopTimer()
{
	m_run_timer = false;
	TimerTick();
}

void WebsockSession::SetIntervalInUs(uint32_t interval)
{
	m_tick_interval_in_us = interval;
}

void WebsockSession::CommsHandler(beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	//TRACE(", sessionID: " << m_sessionID);

	uint8_t* buff = (uint8_t*)static_cast<net::const_buffer>(in_buffer.data()).data();

	CommsHandler(buff, in_length);
}

void WebsockSession::CommitTxBuffer(std::unique_ptr<AppBuffer>& buffer)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	//TRACE(", sessionID: " << m_sessionID);

	m_txQue.push_back(std::move(buffer));
}

bool WebsockSession::GetNextTxBuffer(std::unique_ptr<AppBuffer>& buff)
{
	const std::lock_guard<std::recursive_mutex> lock(m_session_mutex);

	//TRACE(", sessionID: " << m_sessionID);

	if (m_txQue.empty())
		return false;

	// Get next AppBuffer from TX que
	buff = std::move(m_txQue.front());
	m_txQue.pop_front();

	return true;
}

void WebsockSession::OnTimerTick()
{
	auto txBuff = std::make_unique<AppBuffer>(12, m_isLittleEndian);

	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(0x07);
	txBuff->set_uint32(m_sessionID);
	txBuff->set_uint32(m_timer_tick++);

	CommitTxBuffer(txBuff);

	OnTxReady(*this);
}

void WebsockSession::TimerTick()
{
	//TRACE("Timer fired, session id: " << m_sessionID);

	if (!m_timer)
	{
		TRACE("")
		m_timer_complete = true;
		return;
	}

	if (!m_run_timer)
	{
		//TRACE("")
		m_timer_complete = true;
		return;
	}

	OnTimerTick();

	boost::system::error_code ec;
	m_timer->expires_from_now(boost::posix_time::microseconds(m_tick_interval_in_us), ec);
	if (ec)
	{
		TRACE(ec);
		return;
	}

	m_timer->async_wait([this](const boost::system::error_code& e) {
		(void)e;
		TimerTick();
	});
}
