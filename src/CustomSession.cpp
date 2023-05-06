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
	AddTimerTickHandler(std::bind(&CustomSession::HandleTimerTick, this));

	SetIntervalInUs(500000);
}

CustomSession::~CustomSession()
{
	//TRACE("");
}

void CustomSession::HandleNewSession(AppBuffer& rxBuffer)
{
	m_ship = std::make_unique<Asteroids::Ship>(0, 0, 0.0f);

	TRACE(*this);
}

void CustomSession::HandleKeyEvent(AppBuffer& rxBuffer)
{
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
	m_ship->TickEvent();
}

