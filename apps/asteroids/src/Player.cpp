#include "geoff.h"
#include "Consts.h"

#include "RockField.h"

#include "Player.h"
#include "Gun.h"
#include "Universe.h"

//#undef PL_TRACE
//#define PL_TRACE TRACE

using namespace asteroids;

Player::Player(double width, double height)
	:
	Size({ width, height }),
	m_ship(static_cast<uint16_t>(width), static_cast<uint16_t>(height), g_universe->sizeW / 2, g_universe->sizeH / 2, 90 * DEGREES_TO_RADS),
	m_deltaX(1),
	m_deltaY(1),
	m_score(0),
	m_phase(InLobby)
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
	size_t outSize = 24;

	// get properly sized output buffer
	auto txBuff = std::make_unique<AppBuffer>(outSize, session.IsLittleEndian());

	// Message start, Message type, session ID, tick count
	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(static_cast<uint8_t>(WebsockSession::MessageType_t::PlayerTickMessage));
	txBuff->set_uint32(session.SessionID());
	txBuff->set_uint32(g_universe->GetTicks());

	//TRACE(__FUNCTION__ << "ctxW: " << ctxW << ", ctxH: " << ctxH << ", ctxOX: " << ctxOX << ", ctxOY: " << ctxOY);

	// support sliding viewport (allowing ship to move it)
	txBuff->set_uint16(ctxW);
	txBuff->set_uint16(ctxH);
	txBuff->set_uint16(ctxOX);
	txBuff->set_uint16(ctxOY);

	txBuff->set_uint32(m_score);
	txBuff->set_uint16(m_phase);

	return txBuff;
}

void Player::KeyEvent(int key, bool isDown)
{
	m_ship.KeyEvent(key, isDown);

	if (isDown && m_phase != Playing)
	{
		auto old_phase = m_phase;
		m_phase = Playing;
		PL_TRACE("Old phase: " << old_phase << ", New phase: " << m_phase);
		if (m_ship.Dead())
		{
			m_score = 0;
			m_ship.Resurrect();
		}
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
	if (!m_ship.Dead())
	{
		// let Ship Context position move Player context postion
		ctxOX = m_ship.ctxOX;
		ctxOY = m_ship.ctxOY;

		m_ship.TickEvent(session);
	}

	if (m_ship.Dead() && m_phase == Playing)
	{
		m_phase = GameOver;
	}

	// make Ship buffer first, Player viewport (ctx) position is dependency
	auto txShipBuff = m_ship.MakeBuffer(session);
	auto txPlayerTickHeader = MakeBuffer(session);

	auto txBuff = std::make_unique<AppBuffer>(*txPlayerTickHeader, *txShipBuff, session.IsLittleEndian());

	session.CommitTxBuffer(txBuff);
}
