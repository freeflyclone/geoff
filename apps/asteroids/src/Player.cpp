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
	m_left_down(false),
	m_right_down(false),
	m_up_down(false),
	m_down_down(false),
	m_shift_down(false)
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
	PL_TRACE(__FUNCTION__);
}

void Player::KeyEvent(int key, bool isDown)
{
	m_ship.KeyEvent(key, isDown);

	switch (key)
	{
		case 'a':
		case'A':
			//m_ctxDeltaX = (isDown) ? -1 : 0;
			m_left_down = isDown;
			break;

		case 'd':
		case'D':
			m_right_down = isDown;
			//m_ctxDeltaX = (isDown && (ctxOX < g_universe->sizeW - sizeW) ? +1 : 0);
			break;

		case 'w':
		case'W':
			m_up_down = isDown;
			//m_ctxDeltaY = (isDown) ? -1 : 0;
			break;

		case 's':
		case'S':
			m_down_down = isDown;
			//m_ctxDeltaY = (isDown && (ctxOY < g_universe->sizeH - sizeH) ? +1 : 0);
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
	PL_TRACE(__FUNCTION__);

	int universeClickX = clickX + static_cast<int>(Context::ctxOX);
	int universeClickY = clickY + static_cast<int>(Context::ctxOY);

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
	if (ctxOX > 0)
		ctxOX -= m_left_down ? (m_deltaX + (m_shift_down ? 10 : 0)) : 0;

	if (ctxOX < static_cast<uint16_t>(g_universe->sizeW - sizeW))
		ctxOX += m_right_down ? (m_deltaX + (m_shift_down ? 10 : 0)) : 0;

	if (ctxOY > 0)
		ctxOY -= m_up_down ? (m_deltaY + (m_shift_down ? 10 : 0)) : 0;

	if (ctxOY < static_cast<uint16_t>(g_universe->sizeH - sizeH))
		ctxOY += m_down_down ? (m_deltaY + (m_shift_down ? 10 : 0)) : 0;

	m_ship.ctxOX = ctxOX;
	m_ship.ctxOY = ctxOY;

	m_ship.TickEvent(session);

	// make an AppBuffer for user's browser
	size_t outSize = 26;

	// Handle m_bullets from gun
	std::unique_ptr<AppBuffer> bulletsBuffer;
	auto gun = m_ship.GetGun();

	bulletsBuffer = gun->MakeBulletsBuffer(session);
	if (bulletsBuffer.get())
		outSize += bulletsBuffer->size();

	auto txBuff = std::make_unique<AppBuffer>(outSize, session.IsLittleEndian());

	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(static_cast<uint8_t>(WebsockSession::MessageType_t::PlayerTickMessage));
	txBuff->set_uint32(session.SessionID());
	txBuff->set_uint32(g_universe->GetTicks());

	//TRACE(__FUNCTION__ << "ctxW: " << ctxW << ", ctxH: " << ctxH << ", ctxOX: " << ctxOX << ", ctxOY: " << ctxOY);

	txBuff->set_uint16(ctxW);
	txBuff->set_uint16(ctxH);
	txBuff->set_uint16(ctxOX);
	txBuff->set_uint16(ctxOY);

	txBuff->set_uint16(static_cast<uint16_t>(m_ship.posX));
	txBuff->set_uint16(static_cast<uint16_t>(m_ship.posY));
	txBuff->set_uint16(static_cast<uint16_t>(m_ship.angle * FP_4_12));

	// default bullet count to 0
	txBuff->set_uint16(0);

	if (outSize > 26)
	{
		auto offset = txBuff->allocate(static_cast<int>(bulletsBuffer->size()));
		txBuff->set_uint16(24, bulletsBuffer->get_uint16(0));
		memcpy(txBuff->data() + offset, bulletsBuffer->data() + 2, bulletsBuffer->size() - 2);
	}

	session.CommitTxBuffer(txBuff);
}