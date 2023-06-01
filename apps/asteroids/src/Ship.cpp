#define _USE_MATH_DEFINES
#include <cmath>
#include "geoff.h"

#include "Consts.h"

#include "Ship.h"
#include "Gun.h"
#include "Universe.h" 

using namespace asteroids;

#undef SH_TRACE
#define SH_TRACE TRACE

Ship::Ship(uint16_t cw, uint16_t ch, double x, double y, double angle)
	:
	Context({ cw, ch, static_cast<uint16_t>(g_universe->sizeW / 2 - cw / 2), static_cast<uint16_t>(g_universe->sizeH / 2 - ch / 2) }),
	Position({ x, y }),
	Velocity({ 0, 0 }),
	Orientation({ angle }),
	Radius({ SHIP_SIZE / 2 }),
	m_gun(std::make_unique<Gun>()),
	m_rotation(0),
	m_max_delta_v(SHIP_MAX_DELTA_V),
	m_viewport_margin(VIEWPORT_MARGIN),
	m_explosion_duration(SHIP_EXPLODE_DUR),
	m_inv_duration(SHIP_INV_DUR),
	m_blink_time(SHIP_BLINK_DUR * FPS),
	m_blink_num(static_cast<int>(SHIP_INV_DUR / SHIP_BLINK_DUR)),
	m_lives_left(5),
	m_thrusting(false),
	m_left(false),
	m_right(false),
	m_slide_viewport(true),
	m_wrap_viewport(false),
	m_enforce_boundaries(true),
	m_exploding(false),
	m_visible(true),
	m_dead(false),
	m_invulnerable(true)
{
	SH_TRACE(__FUNCTION__);
}

Ship::~Ship()
{
	SH_TRACE(__FUNCTION__);
}

void Ship::FireGuns()
{
	//SH_TRACE(__FUNCTION__);

	if(m_gun)
		m_gun->Fire(*this);
}

void Ship::Explode()
{
	SH_TRACE(__FUNCTION__);
	m_exploding = true;
	deltaX = 0.0;
	deltaY = 0.0;
}

std::unique_ptr<AppBuffer> Ship::MakeBuffer(Session& session)
{
	// make an AppBuffer for user's browser (at least the header)
	size_t outSize = 28;

	// Handle m_bullets from gun
	std::unique_ptr<AppBuffer> bulletsBuffer;

	// bullets present increase total size of output buffer
	bulletsBuffer = m_gun->MakeBuffer(session);
	if (bulletsBuffer.get())
		outSize += bulletsBuffer->size();

	// get properly sized output buffer
	auto txBuff = std::make_unique<AppBuffer>(outSize, session.IsLittleEndian());

	// start with Message start, Message type, session ID, tick count
	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(static_cast<uint8_t>(WebsockSession::MessageType_t::PlayerTickMessage));
	txBuff->set_uint32(session.SessionID());
	txBuff->set_uint32(g_universe->GetTicks());

	//TRACE(__FUNCTION__ << "ctxW: " << ctxW << ", ctxH: " << ctxH << ", ctxOX: " << ctxOX << ", ctxOY: " << ctxOY);

	// support sliding viewport
	txBuff->set_uint16(ctxW);
	txBuff->set_uint16(ctxH);
	txBuff->set_uint16(ctxOX);
	txBuff->set_uint16(ctxOY);

	// ship position, orientation
	txBuff->set_uint16(static_cast<uint16_t>(posX));
	txBuff->set_uint16(static_cast<uint16_t>(posY));
	txBuff->set_uint16(static_cast<uint16_t>(angle * FP_4_12));

	// ship's flags word
	uint16_t flags = m_thrusting ? 1 : 0;
	flags |= m_exploding ? 0x02 : 0;
	flags |= m_visible ? 0x04 : 0;
	flags |= m_dead ? 0x08 : 0;

	txBuff->set_uint16(flags);

	// default bullet count: 0 (overwritten if #bullets > 0)
	txBuff->set_uint16(0);

	// are there bullets?
	if (outSize > 28)
	{
		// change bytesWritten, overwrite bullet count, add bullet data
		auto offset = txBuff->allocate(static_cast<int>(bulletsBuffer->size()));
		txBuff->set_uint16(26, bulletsBuffer->get_uint16(0));
		memcpy(txBuff->data() + offset, bulletsBuffer->data() + 2, bulletsBuffer->size() - 2);
	}

	return txBuff;
}

void Ship::KeyEvent(int key, bool isDown)
{
	(void)key;
	(void)isDown;

	//SH_TRACE(__FUNCTION__);

	switch (key)
	{
	case 'A':
	case 37:
		m_left = isDown;
		break;

	case 'D':
	case 39:
		m_right = isDown;
		break;

	case 'I':
	case 'W':
	case 38:
		m_thrusting = isDown;
		break;

	case 32:
			if (isDown)
				FireGuns();
			break;
	}
}

void Ship::ResizeEvent(int w, int h)
{
	ctxW = static_cast<uint16_t>(w);
	ctxH = static_cast<uint16_t>(h);
	ctxOX = static_cast<uint16_t>(g_universe->sizeW / 2) - ctxW / 2;
	ctxOY = static_cast<uint16_t>(g_universe->sizeH / 2) - ctxH / 2;

	SH_TRACE(__FUNCTION__ << ", ctxW: " << ctxW << ", ctxH: " << ctxH << ", ctxOX: " << ctxOX << ", ctxOY: " << ctxOY);
}

void Ship::TickEvent(Session& session)
{
	//SH_TRACE(__FUNCTION__);
	MoveShip();

	if(m_gun)
		m_gun->TickEvent(session);

	if (m_exploding)
	{
		m_explosion_duration -= 1 / FPS;
		if (m_explosion_duration < 0)
		{
			m_explosion_duration = SHIP_EXPLODE_DUR;
			m_exploding = false;
			TRACE("Explosion done.");
			NewLife();
		}
	}
	else if (m_invulnerable)
	{
		m_inv_duration -= 1 / FPS;
		if (m_inv_duration < 0)
		{
			m_invulnerable = false;
			TRACE("Vulnerable now.");
		}
	}
}

void Ship::NewLife()
{
	if (m_lives_left)
	{
		m_invulnerable = true;
		m_explosion_duration = SHIP_EXPLODE_DUR;
		m_inv_duration = SHIP_INV_DUR;
		m_blink_time = SHIP_BLINK_DUR * FPS;
		m_blink_num = static_cast<int>(SHIP_INV_DUR / SHIP_BLINK_DUR);
		m_lives_left--;
	}
	else
	{
		TRACE("No lives left");
		m_dead = true;
	}
}

void Ship::MoveShip()
{
	//SH_TRACE(__FUNCTION__ << "Ship: " << posX << "," << posY << " - " << deltaX << "," << deltaY << ", angle: " << angle << ", " << (m_thrusting ? "thrusting" : ""));

	if (m_exploding || m_dead)
		return;

	m_rotation = 0;
	m_rotation += m_left ? SHIP_RADS_PER_TICK : 0;
	m_rotation -= m_right ? SHIP_RADS_PER_TICK : 0;

	if (m_thrusting)
	{
		// accelerate
		auto thrustX = (double)  SHIP_THRUST * cos(angle) / (double)FPS;
		auto thrustY = (double) -SHIP_THRUST * sin(angle) / (double)FPS;

		deltaX += thrustX;
		deltaY += thrustY;

		if (deltaX > m_max_delta_v)
			deltaX = m_max_delta_v;
		if (deltaX < -m_max_delta_v)
			deltaX = -m_max_delta_v;

		if (deltaY > m_max_delta_v)
			deltaY = m_max_delta_v;
		if (deltaY < -m_max_delta_v)
			deltaY = -m_max_delta_v;


		//SH_TRACE("Velockity: " << deltaX << "," << deltaY);
	}
	else
	{
		// decelerate
		deltaX -= FRICTION * deltaX / FPS;
		deltaY -= FRICTION * deltaY / FPS;

		//SH_TRACE("Velockity: " << deltaX << "," << deltaY);
	}

	if (m_slide_viewport)
	{
		// left?
		if (posX < (ctxOX + m_viewport_margin))
		{
			if (posX > m_viewport_margin)
				ctxOX = posX - m_viewport_margin;
		}
		// right?
		else if (posX > (ctxOX + ctxW - m_viewport_margin))
		{
			if (posX < g_universe->sizeW - m_viewport_margin)
				ctxOX = posX - ctxW + m_viewport_margin;
		}

		// up?
		if (posY < (ctxOY + m_viewport_margin))
		{
			if (posY > m_viewport_margin)
				ctxOY = posY - m_viewport_margin;
		}
		// down?
		else if (posY > (ctxOY + ctxH - m_viewport_margin))
		{
			if (posY < g_universe->sizeH - m_viewport_margin)
				ctxOY = posY - ctxH + m_viewport_margin;
		}
	}

	// Rotation is tracked in radians, and we want to clip the rotation
	// angle like we do for screen edges.  That is: keep it between 0
	// and 360 degrees, (in radians of course)
	auto max_angle = M_PI * 2.0;

	// m_rotation toggled in keypress handler
	angle += m_rotation;

	// auto-wrap rotation: ie keep between 0-360 degrees (in radians)
	if (angle >= max_angle)
		angle = 0;
	else if (angle < 0)
		angle = max_angle;

	// apply thrust or friction
	posX += deltaX;
	posY += deltaY;

	if (m_enforce_boundaries)
	{
		if (posX < radius)
		{
			posX = radius;
			deltaX = 0.0;
		}
		else if (posX + radius >= g_universe->sizeW)
		{
			posX = g_universe->sizeW - radius;
			deltaX = 0.0;
		}

		if (posY < radius)
		{
			posY = radius;
			deltaY = 0.0;
		}
		else if (posY + radius >= g_universe->sizeH)
		{
			posY = g_universe->sizeH - radius;
			deltaY = 0.0;
		}
	}

	if (m_wrap_viewport)
	{
		// Get Context dimensions & offset as doubles (not uint16_t)
		auto offX = static_cast<double>(ctxOX);
		auto offY = static_cast<double>(ctxOY);
		auto windowW = static_cast<double>(ctxW);
		auto windowH = static_cast<double>(ctxH);

		// handle wrapping at edge of screen
		// (eventually: move the Context within the Universe)a
		if (posX < offX) {
			posX = offX + windowW;
		}
		else if (posX > offX + windowW) {
			posX = offX;
		}

		if (posY < offY) {
			posY = offY + windowH;
		}
		else if (posY > offY + windowH) {
			posY = offY;
		}
	}
}

