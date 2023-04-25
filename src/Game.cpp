#include "Game.h"

#include <ios>
#include <iostream>

Game::Game() :
	m_playersMutex(),
	m_clientID(0),
	m_clients()
{

	srand(12345);

	std::cout << "geoff server ver " << gameAppVersion << " running\n";
}

Game::~Game()
{
}

// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
Game& Game::GetInstance()
{
	static Game g;
	return g;
}

void Game::RegisterNewClientConnection(AppBuffer & rxBuffer)
{
	uint16_t clientAppVersion = rxBuffer.get_uint16();

	auto txBuffer = std::make_shared<AppBuffer>(10, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x09);
	txBuffer->set_uint16(m_clientID);
	txBuffer->set_uint16((uint16_t)gameAppVersion);
	txBuffer->set_uint16(m_mapWidth);
	txBuffer->set_uint16(m_mapHeight);

	m_clients.push_back(std::make_shared<Client>(m_clientID, rxBuffer.isLittleEndian()));
	m_txQue.push_back(txBuffer);

	std::cout << "   Client Ver: " << clientAppVersion << " connected, assigned #" << m_clientID << "\n";

	m_clientID++;
	m_clientID &= 32767;	
}

void Game::HandleKeyEvent(AppBuffer & rxBuffer)
{
	bool isDown = (rxBuffer.get_uint8() == 1) ? true : false;
	int keyCode = rxBuffer.get_uint8();

	std::cout << "Action: " << (isDown ? "Down" : "Up") <<  ", ";
	std::cout << "keyCode: 0x" << std::hex << keyCode << std::endl;
}

void Game::CommsHandler(beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::mutex> lock(m_playersMutex);

	if (in_length < 2)
		return;

	uint8_t* buff = (uint8_t*)static_cast<net::const_buffer>(in_buffer.data()).data();

	if (buff[0] == 0xAA || buff[0] == 0xAB)
	{
		bool isLittleEndian = buff[0] == 0xAB ? true : false;
		Game::RequestType_t request = static_cast<Game::RequestType_t>(buff[1]);

		// skip header
		buff += 2;
		in_length -= 2;

		AppBuffer rxBuffer = AppBuffer(buff, in_length, isLittleEndian);

		switch (request)
		{
			case RegisterClient:
				RegisterNewClientConnection(rxBuffer);
				break;

			case KeyEvent:
				HandleKeyEvent(rxBuffer);
				break;
		}
	}
}

bool Game::GetNextTxBuffer(std::shared_ptr<AppBuffer> & buff)
{
	if (m_txQue.empty())
		return false;

	// Get next AppBuffer from TX que
	buff = m_txQue.front();

	return true;
}
