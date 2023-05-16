#include "geoff.h"
#include "WebsockSessionManager.h"

#include "Session.h"
#include "Player.h"
#include "Universe.h"

using namespace as2;

Session::Session(uint32_t sessionID)
	:
	GameSession(sessionID),
	m_player(nullptr),
	m_timer(*this, 50000)
{
	SS_TRACE(__FUNCTION__);
	Init(8192, 8192);
}

Session::~Session()
{
	SS_TRACE(__FUNCTION__);
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

void Session::TickEvent()
{
	SS_TRACE(__FUNCTION__);

	if (g_universe)
		g_universe->TickEvent(*this);

	if (m_player)
		m_player->TickEvent(*this);
}
