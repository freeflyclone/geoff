#include "CustomSession.h"
#include "Asteroids.h"

//#define CS_TRACE TRACE
#define CS_TRACE(...)

std::ostream& operator<<(std::ostream& os, const CustomSession& cs)
{
	os << std::endl << ">>> CustomSession" << std::endl;
	os << "SessionID: " << cs.m_sessionID << std::endl;
	os << "<<< CustomSession" << std::endl;
	return os;
}

CustomSession::CustomSession(uint32_t sessionID)
	: 
	GameSession(sessionID)
{
	CS_TRACE("");
}

CustomSession::~CustomSession()
{
	CS_TRACE("");
}

void CustomSession::HandleNewSession(AppBuffer& rxBuffer)
{
	GameSession::HandleNewSession(rxBuffer);

	auto w = rxBuffer.get_uint16(3);
	auto h = rxBuffer.get_uint16(5);

	m_ship = std::make_unique<Asteroids::Ship>(w, h, w / 2, h / 2, static_cast<float>(M_PI / 2.0f));

	CS_TRACE(*this);
}

void CustomSession::HandleKeyEvent(AppBuffer& rxBuffer)
{
	GameSession::HandleKeyEvent(rxBuffer);

	if (!m_ship)
		return;

	CS_TRACE("KeyEvent");

	bool isDown = rxBuffer.get_uint8(1);
	int key     = (int)rxBuffer.get_uint8(2);

	m_ship->KeyEvent(key, isDown);
}

void CustomSession::HandleClickEvent(AppBuffer& rxBuffer)
{
	GameSession::HandleClickEvent(rxBuffer);

	CS_TRACE("ClickEvent");
}

void CustomSession::HandleTimerTick()
{
	GameSession::HandleTimerTick();
	CS_TRACE("TimerTick");

	if (!m_ship)
		return;

	m_ship->TickEvent();
	int16_t shipX, shipY, shipA;

	m_ship->GetXY(shipX, shipY);
	m_ship->GetAngle(shipA);

	size_t outSize = 18;

	// Handle bullets from gun
	std::unique_ptr<AppBuffer> bulletsBuffer;
	auto gun = m_ship->m_gun;

	// ensure gun actually exists
	if (gun)
	{
		// Get size of bullet buffer (if any bullets are active)
		// and adjust "outsize" to allow room for bullets
		bulletsBuffer = std::move(gun->MakeBulletsPacket(IsLittleEndian()));
		if (bulletsBuffer.get())
			outSize += bulletsBuffer->size();
	}

	auto txBuff = std::make_unique<AppBuffer>(outSize, m_isLittleEndian);

	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(0x07);
	txBuff->set_uint32(m_sessionID);
	txBuff->set_uint32(m_timer_tick++);
	txBuff->set_uint16(shipX);
	txBuff->set_uint16(shipY);
	txBuff->set_uint16(shipA);

	// default bullet count to 0
	txBuff->set_uint16(0);

	if (outSize > 18)
	{
		auto offset = txBuff->allocate(static_cast<int>(bulletsBuffer->size()));
		txBuff->set_uint16(16, bulletsBuffer->get_uint16(0));
		memcpy(txBuff->data() + offset, bulletsBuffer->data() + 2, bulletsBuffer->size() - 2);
		
		//TRACE("");
	}

	CommitTxBuffer(txBuff);

	OnTxReady(*this);
}

void CustomSession::HandleResizeEvent(AppBuffer& rxBuffer)
{
	GameSession::HandleResizeEvent(rxBuffer);

	uint32_t sessionID = rxBuffer.get_uint32(1);

	uint16_t width = rxBuffer.get_uint16(5);
	uint16_t height = rxBuffer.get_uint16(7);

	CS_TRACE("sessionID: " << sessionID << ", width: " << width << ", height: " << height);

	m_ship->Resize(width, height);
}

