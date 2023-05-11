#include "AsteroidsSession.h"
#include "Asteroids.h"

//#define AS_TRACE TRACE
#define AS_TRACE(...)

// Friend function for providing << stream operator for AsteroidsSession
// class.
std::ostream& operator<<(std::ostream& os, const AsteroidsSession& as)
{
	os << std::endl << ">>> AsteroidsSession" << std::endl;
	os << "\tSessionID: " << as.m_sessionID << std::endl;
	os << "\t   Player: " << (as.m_player ? "Found" : "Not yet") << std::endl;
	os << "\t Universe: " << (as.m_universe ? "Found" : "Not yet") << std::endl;
	os << "<<< AsteroidsSession";
	return os;
}

AsteroidsSession::AsteroidsSession(uint32_t sessionID)
	: 
	GameSession(sessionID)
{
	// example of using the custom '<<' stream operator
	TRACE(*this);
}

AsteroidsSession::~AsteroidsSession()
{
	AS_TRACE("");
}

double AsteroidsSession::DistanceBetweenPoints(Asteroids::Position& point1, Asteroids::Position& point2)
{
	auto dxSquared = std::pow(point2.x - point1.x, 2);
	auto dySquared = std::pow(point2.y - point1.y, 2);

	return std::sqrt(dxSquared + dySquared);
}

void AsteroidsSession::HandleNewSession(AppBuffer& rxBuffer)
{
	GameSession::HandleNewSession(rxBuffer);

	auto w = rxBuffer.get_uint16(3);
	auto h = rxBuffer.get_uint16(5);

	// we have to wait for the client to register, in order to get window width and height.
	m_player = std::make_unique<Asteroids::Player>(*this, w, h);
	m_universe = std::make_unique<Asteroids::Universe>(*this, w, h);

	AS_TRACE(*this);

	StartTimer();
	SetIntervalInUs(1000000 / Asteroids::FPS);

	//SetIntervalInUs(1000000);
}

void AsteroidsSession::HandleKeyEvent(AppBuffer& rxBuffer)
{
	AS_TRACE("KeyEvent");

	bool isDown = rxBuffer.get_uint8(1);
	int key     = (int)rxBuffer.get_uint8(2);

	if (m_universe)
		m_universe->KeyEvent(key, isDown);

	if (m_player)
		m_player->KeyEvent(key, isDown);
}

// Asteroids currently doesn't care about click events. May change.
void AsteroidsSession::HandleClickEvent(AppBuffer& rxBuffer)
{
	// preserve rxBuffer state for possible additional handlers
	// (ie read data without altering its m_readOffset)
	assert(rxBuffer.size() >= 11);

	uint32_t rxSessionID = rxBuffer.get_uint32(1);
	uint16_t clickX = rxBuffer.get_uint16(5);
	uint16_t clickY = rxBuffer.get_uint16(7);


	AS_TRACE("ClickEvent");

	if (m_universe)
		m_universe->ClickEvent(clickX, clickY);
}

void AsteroidsSession::HandleResizeEvent(AppBuffer& rxBuffer)
{
	GameSession::HandleResizeEvent(rxBuffer);

	uint16_t width = rxBuffer.get_uint16(5);
	uint16_t height = rxBuffer.get_uint16(7);

	AS_TRACE("sessionID: " << SessionID() << ", width: " << width << ", height: " << height);

	if (m_universe)
		m_universe->ResizeEvent(width, height);

	if (m_player)
		m_player->ResizeEvent(width, height);
}

void AsteroidsSession::HandleTimerTick()
{
	AS_TRACE("TimerTick");

	if (m_universe)
		m_universe->TickEvent(*this);

	if (m_player)
		m_player->TickEvent(*this);

	OnTxReady(*this);
}
