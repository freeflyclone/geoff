#include "CustomSession.h"
#include "Asteroids.h"

std::ostream& operator<<(std::ostream& os, const CustomSession& cs)
{
	os << std::endl << ">>> CustomSession" << std::endl;
	os << "SessionID: " << cs.m_sessionID << std::endl;
	os << "<<< CustomSession" << std::endl;
	return os;
}

CustomSession::CustomSession(uint32_t sessionID)
	: GameSession(sessionID)
{
	//TRACE("");

	AddRegisterNewSessionHandler(std::bind(&CustomSession::HandleNewSession, this, std::placeholders::_1));
	AddKeyEventHandler(std::bind(&CustomSession::HandleKeyEvent, this, std::placeholders::_1));
	AddClickEventHandler(std::bind(&CustomSession::HandleClickEvent, this, std::placeholders::_1));
	AddResizeEventHandler(std::bind(&CustomSession::HandleResizeEvent, this, std::placeholders::_1));
	AddTimerTickHandler(std::bind(&CustomSession::HandleTimerTick, this));

	//SetIntervalInUs(500000);
}

CustomSession::~CustomSession()
{
	//TRACE("");
}

void CustomSession::HandleNewSession(AppBuffer& rxBuffer)
{
	auto w = rxBuffer.get_uint16(3);
	auto h = rxBuffer.get_uint16(5);

	m_ship = std::make_unique<Asteroids::Ship>(w, h, w / 2, h / 2, static_cast<float>(M_PI / 2.0f));

	TRACE(*this);
}

void CustomSession::HandleKeyEvent(AppBuffer& rxBuffer)
{
	if (!m_ship)
		return;

	bool isDown = rxBuffer.get_uint8(1);
	int key     = (int)rxBuffer.get_uint8(2);

	m_ship->KeyEvent(key, isDown);
}

void CustomSession::HandleClickEvent(AppBuffer& rxBuffer)
{
	//TRACE("");
}

void CustomSession::HandleTimerTick()
{
	if (!m_ship)
		return;

	m_ship->TickEvent();
	int16_t shipX, shipY, shipA;

	m_ship->GetXY(shipX, shipY);
	m_ship->GetAngle(shipA);

	auto txBuff = std::make_unique<AppBuffer>(16, m_isLittleEndian);

	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(0x07);
	txBuff->set_uint32(m_sessionID);
	txBuff->set_uint32(m_timer_tick++);
	txBuff->set_uint16(shipX);
	txBuff->set_uint16(shipY);
	txBuff->set_uint16(shipA);

	CommitTxBuffer(txBuff);
	OnTxReady(*this);
}

void CustomSession::HandleResizeEvent(AppBuffer& rxBuffer)
{
	uint32_t sessionID = rxBuffer.get_uint32(1);
	uint16_t width = rxBuffer.get_uint16(5);
	uint16_t height = rxBuffer.get_uint16(7);

	//TRACE("sessionID: " << sessionID << ", width: " << width << ", height: " << height);

	m_ship->Resize(width, height);
}

