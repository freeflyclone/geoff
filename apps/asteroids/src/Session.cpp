#include "geoff.h"
#include "WebsockSessionManager.h"

#include "Consts.h"
#include "Session.h"
#include "Player.h"
#include "Universe.h"
#include "AppSession.h"

using namespace asteroids;

Session::Session(uint32_t sessionID)
	:
	GameSession(sessionID),
	m_sessionID(sessionID),
	m_player(nullptr)
{
	SS_TRACE(__FUNCTION__);

	GetUniverse();
}

Session::~Session()
{
	SS_TRACE(__FUNCTION__);
}

double Session::DistanceBetweenPoints(Position& p1, Position& p2)
{
	auto dxSquared = std::pow(p2.posX - p1.posX, 2);
	auto dySquared = std::pow(p2.posY - p1.posY, 2);

	return std::sqrt(dxSquared + dySquared);
}

void Session::HandleNewSession(AppBuffer& rxBuffer)
{
	SS_TRACE(__FUNCTION__);

	auto w = rxBuffer.get_uint16(3);
	auto h = rxBuffer.get_uint16(5);

	m_player = std::make_unique<Player>(*this, static_cast<double>(w), static_cast<double>(h));
}

void Session::HandleKeyEvent(AppBuffer& rxBuffer)
{
	SS_TRACE(__FUNCTION__);

	int key = static_cast<int>(rxBuffer.get_uint8(2));
	int isDown = static_cast<bool>(rxBuffer.get_uint8(1));

	if(m_player)
		m_player->KeyEvent(key, isDown);
}

void Session::HandleClickEvent(AppBuffer& rxBuffer)
{
	SS_TRACE(__FUNCTION__);

	int clickX = static_cast<int>(rxBuffer.get_uint16(5));
	int clickY = static_cast<int>(rxBuffer.get_uint16(7));

	if (m_player)
		m_player->ClickEvent(clickX, clickY);
}

void Session::HandleResizeEvent(AppBuffer& rxBuffer)
{
	SS_TRACE(__FUNCTION__);

	int width = static_cast<int>(rxBuffer.get_uint16(5));
	int height = static_cast<int>(rxBuffer.get_uint16(7));

	SS_TRACE("sessionID: " << SessionID() << ", width: " << width << ", height: " << height);

	if (m_player)
		m_player->ResizeEvent(width, height);
}

void Session::TickEvent(uint32_t sessionID, uint32_t tickCount)
{
	(void)sessionID;
	(void)tickCount;

	SS_TRACE(__FUNCTION__);

	if (m_player)
		m_player->TickEvent(*this);

	OnTxReady(*this);
}
