#include "geoff.h"
#include "Consts.h"

#include "RockField.h"

#include "Player.h"
#include "Gun.h"
#include "Universe.h"

//#undef PL_TRACE
//#define PL_TRACE TRACE

using namespace asteroids;

Player::Player(Session& session, double width, double height)
	:
	Size({ width, height }),
	m_session(session),
	m_ship(static_cast<uint16_t>(width), static_cast<uint16_t>(height), g_universe->sizeW / 2, g_universe->sizeH / 2, 90 * DEGREES_TO_RADS),
	m_deltaX(1),
	m_deltaY(1),
	m_score(0),
	m_left_down(false),
	m_right_down(false),
	m_up_down(false),
	m_down_down(false),
	m_shift_down(false),
	m_manual_viewport(false)
{
	// initialize Context: our browser's window size and offset within the g_universe virtual size
	ctxW = static_cast<uint16_t>(width);
	ctxH = static_cast<uint16_t>(height);
	ctxOX = static_cast<uint16_t>(g_universe->sizeW / 2) - ctxW / 2;
	ctxOY = static_cast<uint16_t>(g_universe->sizeH / 2) - ctxH / 2;

	PL_TRACE(__FUNCTION__);
}

Player::~Player()
{
	TRACE(__FUNCTION__);
}

void Player::AddToScore(uint32_t increment)
{
	m_score += increment;
}

std::unique_ptr<AppBuffer> Player::MakeBuffer(Session& session)
{
	// make an AppBuffer(PlayerTickMessage) header for user's JS engine
	size_t outSize = 22;

	// get properly sized output buffer
	auto txBuff = std::make_unique<AppBuffer>(outSize, session.IsLittleEndian());

	// Message start, Message type, session ID, tick count
	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(static_cast<uint8_t>(WebsockSession::MessageType_t::PlayerTickMessage));
	txBuff->set_uint32(session.SessionID());
	txBuff->set_uint32(g_universe->GetTicks());

	//TRACE(__FUNCTION__ << "ctxW: " << ctxW << ", ctxH: " << ctxH << ", ctxOX: " << ctxOX << ", ctxOY: " << ctxOY);

	// support sliding viewport (allowing ship to move it)
	txBuff->set_uint16(m_ship.ctxW);
	txBuff->set_uint16(m_ship.ctxH);
	txBuff->set_uint16(m_ship.ctxOX);
	txBuff->set_uint16(m_ship.ctxOY);

	txBuff->set_uint32(m_score);

	return txBuff;
}

void Player::KeyEvent(int key, bool isDown)
{
	m_ship.KeyEvent(key, isDown);

	switch (key)
	{
		case 'h':
		case'H':
			m_left_down = isDown;
			break;

		case 'l':
		case'L':
			m_right_down = isDown;
			break;

		case 'k':
		case'K':
			m_up_down = isDown;
			break;

		case 'j':
		case'J':
			m_down_down = isDown;
			break;

		case 16:
			m_shift_down = isDown;
			break;

		default:
			PL_TRACE("Key: " << key << ", isDown: " << (isDown ? "true" : "false"));
			break;
	}
}

void Player::ClickEvent(int clickX, int clickY)
{
	int universeClickX = clickX + static_cast<int>(m_ship.ctxOX);
	int universeClickY = clickY + static_cast<int>(m_ship.ctxOY);

	PL_TRACE("clickX: " << clickX << ", clickY: " << clickY << "ucX: " << universeClickX << ", ucY: " << universeClickY);

	g_universe->GetRockField()->LaunchOne(universeClickX, universeClickY, ROCK_RADIUS);
}

void Player::ResizeEvent(int w, int h)
{
	sizeW = static_cast<double>(w);
	sizeH = static_cast<double>(h);

	ctxW = static_cast<uint16_t>(w);
	ctxH = static_cast<uint16_t>(h);
	ctxOX = static_cast<uint16_t>(g_universe->sizeW / 2) - ctxW / 2;
	ctxOY = static_cast<uint16_t>(g_universe->sizeH / 2) - ctxH / 2;

	m_ship.ResizeEvent(w, h);

	//PL_TRACE(__FUNCTION__ << ", sizeW: " << sizeW << ", sizeH: " << sizeH << ", ctxOX: " << ctxOX << ", ctxOY: " << ctxOY);
}

void Player::TickEvent(Session& session)
{
	// see KeyEvent - tl;dr vi navigation keys, shift = boost
	if (m_manual_viewport)
	{
		int16_t deltaX = m_deltaX + (m_shift_down ? 10 : 0);
		int16_t deltaY = m_deltaY + (m_shift_down ? 10 : 0);

		if (ctxOX > deltaX)
			ctxOX -= m_left_down ? deltaX : 0;

		if (ctxOX < static_cast<uint16_t>(g_universe->sizeW - sizeW - deltaX))
			ctxOX += m_right_down ? deltaX : 0;

		if (ctxOY > deltaY)
			ctxOY -= m_up_down ? deltaY : 0;

		if (ctxOY < static_cast<uint16_t>(g_universe->sizeH - sizeH - deltaY))
			ctxOY += m_down_down ? deltaY : 0;

		m_ship.ctxOX = ctxOX;
		m_ship.ctxOY = ctxOY;
	}

	m_ship.TickEvent(session);

	// make Ship buffer first, Player viewport (ctx) position is dependency
	auto txShipBuff = m_ship.MakeBuffer(session);
	auto txPlayerTickHeader = MakeBuffer(session);

	auto txBuff = std::make_unique<AppBuffer>(*txPlayerTickHeader, *txShipBuff, session.IsLittleEndian());

	session.CommitTxBuffer(txBuff);
}
