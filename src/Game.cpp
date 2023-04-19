#include "Game.h"

#include <ios>
#include <iostream>

Game::Game() :
	m_playersMutex(),
	m_clientID(0)
{

	srand(12345);

	std::cout << "geoff server ver " << gameAppVersion << " running\n";
}

Game::~Game()
{
}

void Game::RegisterNewClientConnection(AppBuffer & rxBuffer)
{
}

void Game::HandleKeyEvent(AppBuffer & rxBuffer)
{
	std::cout << "1st byte: 0x" << std::hex << (int)rxBuffer.get_uint8() << ", ";
	std::cout << "2nd byte: 0x" << std::hex << (int)rxBuffer.get_uint8() << std::endl;
}

std::size_t Game::CommsHandler(beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::mutex> lock(m_playersMutex);

	if (in_length < 2)
		return 0;

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

	return in_length;
}

// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
Game& Game::GetInstance()
{
	static Game g;
	return g;
}
