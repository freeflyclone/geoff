#include "AsteroidsSession.h"
#include "Asteroids.h"

//#define CS_TRACE TRACE
#define CS_TRACE(...)

// Friend function for providing << stream operator for AsteroidsSession
// class.
std::ostream& operator<<(std::ostream& os, const AsteroidsSession& as)
{
	os << std::endl << ">>> AsteroidsSession" << std::endl;
	os << "\tSessionID: " << as.m_sessionID << std::endl;
	os << "\t     Ship: " << (as.m_ship ? "Found" : "Not yet") << std::endl;
	os << "<<< AsteroidsSession";
	return os;
}

AsteroidsSession::AsteroidsSession(uint32_t sessionID)
	: 
	GameSession(sessionID)
{
	// example of using the custom '<<' stream operator
	TRACE(*this);
}

AsteroidsSession::~AsteroidsSession()
{
	CS_TRACE("");
}

void AsteroidsSession::HandleNewSession(AppBuffer& rxBuffer)
{
	GameSession::HandleNewSession(rxBuffer);

	auto w = rxBuffer.get_uint16(3);
	auto h = rxBuffer.get_uint16(5);

	m_ship = std::make_unique<Asteroids::Ship>(w, h, w / 2, h / 2, static_cast<float>(M_PI / 2.0f));

	CS_TRACE(*this);

	StartTimer();
	SetIntervalInUs(1000000 / Asteroids::FPS);
}

void AsteroidsSession::HandleKeyEvent(AppBuffer& rxBuffer)
{
	if (!m_ship)
		return;

	CS_TRACE("KeyEvent");

	bool isDown = rxBuffer.get_uint8(1);
	int key     = (int)rxBuffer.get_uint8(2);

	m_ship->KeyEvent(key, isDown);
}

void AsteroidsSession::HandleClickEvent(AppBuffer& rxBuffer)
{
	(void)rxBuffer;
	if (!m_ship)
		return;

	CS_TRACE("ClickEvent");
}

void AsteroidsSession::HandleTimerTick()
{
	CS_TRACE("TimerTick");

	if (!m_ship)
		return;

	m_ship->TickEvent();

	auto txBuffer = NewSessionTickBuffer();

	if (txBuffer)
		CommitTxBuffer(txBuffer);

	OnTxReady(*this);
}

void AsteroidsSession::HandleResizeEvent(AppBuffer& rxBuffer)
{
	GameSession::HandleResizeEvent(rxBuffer);

	uint16_t width = rxBuffer.get_uint16(5);
	uint16_t height = rxBuffer.get_uint16(7);

	CS_TRACE("sessionID: " << SessionID() << ", width: " << width << ", height: " << height);

	if (!m_ship)
		return;

	m_ship->Resize(width, height);
}

std::unique_ptr<AppBuffer> AsteroidsSession::NewSessionTickBuffer()
{
	int16_t shipX, shipY, shipA;

	if (!m_ship)
		return nullptr;

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

	return txBuff;
}

