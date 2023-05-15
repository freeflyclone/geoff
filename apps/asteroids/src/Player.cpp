#include "geoff.h"

#include "Player.h"

using namespace as2;

Player::Player(Session& session, double width, double height)
	:
	Size({ width, height }),
	m_session(session),
	m_ship(static_cast<uint16_t>(width), static_cast<uint16_t>(height), 0, 0, 90)
{
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

void Player::ClickEvent(int x, int y)
{
	PL_TRACE(__FUNCTION__);
}

void Player::ResizeEvent(int w, int h)
{
	PL_TRACE(__FUNCTION__);
}

void Player::TickEvent(Session& session)
{
	PL_TRACE(__FUNCTION__);

	m_ship.TickEvent(session);
}