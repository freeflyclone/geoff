#include "geoff.h"
#include "Consts.h"

#include "RockField.h"

#include "Player.h"
#include "Universe.h"

//#undef PL_TRACE
//#define PL_TRACE TRACE

using namespace as2;

Player::Player(Session& session, double width, double height)
	:
	Size({ width, height }),
	m_session(session),
	m_ship(static_cast<uint16_t>(width), static_cast<uint16_t>(height), g_universe->sizeW / 2, g_universe->sizeH / 2, 90)
{
	ctxW = static_cast<uint16_t>(width);
	ctxH = static_cast<uint16_t>(height);
	ctxOX = static_cast<uint16_t>(g_universe->sizeW / 2) - ctxW / 2;
	ctxOY = static_cast<uint16_t>(g_universe->sizeH / 2) - ctxH / 2;

	PL_TRACE(__FUNCTION__);
}

Player::~Player()
{
	PL_TRACE(__FUNCTION__);
}

void Player::KeyEvent(int key, bool isDown)
{
	PL_TRACE(__FUNCTION__);
	m_ship.KeyEvent(key, isDown);
}

void Player::ClickEvent(int clickX, int clickY)
{
	PL_TRACE(__FUNCTION__);

	int universeClickX = clickX + static_cast<int>(Context::ctxOX);
	int universeClickY = clickY + static_cast<int>(Context::ctxOY);

	g_universe->GetRockField().LaunchOne(universeClickX, universeClickY, ROCK_RADIUS);
}

void Player::ResizeEvent(int w, int h)
{
	sizeW = static_cast<double>(w);
	sizeH = static_cast<double>(h);

	ctxW = static_cast<uint16_t>(w);
	ctxH = static_cast<uint16_t>(h);
	ctxOX = static_cast<uint16_t>(g_universe->sizeW / 2) - ctxW / 2;
	ctxOY = static_cast<uint16_t>(g_universe->sizeH / 2) - ctxH / 2;

	//PL_TRACE(__FUNCTION__ << ", sizeW: " << sizeW << ", sizeH: " << sizeH << ", ctxOX: " << ctxOX << ", ctxOY: " << ctxOY);
}

void Player::TickEvent(Session& session)
{
	PL_TRACE(__FUNCTION__);

	m_ship.TickEvent(session);
}