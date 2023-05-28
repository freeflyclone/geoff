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
	TRACE(__FUNCTION__);
}

void Session::HandleNewSession(AppBuffer& rxBuffer)
{
	SS_TRACE(__FUNCTION__);

	auto universe = GetUniverse();

	auto w = rxBuffer.get_uint16(3);
	auto h = rxBuffer.get_uint16(5);

	if (universe)
	{
		universe->NewPlayer(*this, w, h);
		m_player = universe->GetPlayerById(m_sessionID);
		assert(m_player.get());

		Universe::StarField_t* stars = universe->GetStarField();
		auto numStars = stars->size();

		auto txSize = 8 + sizeof(uint16_t) + (numStars * 2 * sizeof(uint16_t));
		auto txBuffer = std::make_unique<AppBuffer>(txSize, rxBuffer.isLittleEndian());

		txBuffer->set_uint8(0xBB);
		txBuffer->set_uint8(static_cast<uint8_t>(MessageType_t::SessionRegistered));
		txBuffer->set_uint32(SessionID());
		txBuffer->set_uint16((uint16_t)GAME_APP_VERSION);

		txBuffer->set_uint16((uint16_t)numStars);
		for (auto star : *stars)
		{
			txBuffer->set_uint16(static_cast<uint16_t>(star.posX));
			txBuffer->set_uint16(static_cast<uint16_t>(star.posY));
		}

		CommitTxBuffer(txBuffer);
	}
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

double asteroids::DistanceBetweenPoints(Position& p1, Position& p2)
{
	auto dxSquared = std::pow(p2.posX - p1.posX, 2);
	auto dySquared = std::pow(p2.posY - p1.posY, 2);

	return std::sqrt(dxSquared + dySquared);
}
