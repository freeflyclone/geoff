#include "Game.h"

#include <ios>
#include <iostream>

Game::Game()
{

	srand(12345);

	std::cout << "geoff server ver " << gameAppVersion << " running\n";
}

Game::~Game()
{
}

std::size_t Game::CommsHandler(beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::mutex> lock(m_playersMutex);

	if (in_length < 2)
		return 0;

	auto buff = (unsigned char*)static_cast<net::const_buffer>(in_buffer.data()).data();

	std::cout << "1st byte: " << std::hex << (int)buff[0] << ", ";
	std::cout << "2nd byte: " << std::hex << (int)buff[1] << ", ";
	std::cout << "3rd byte: '" << buff[2] << "'" << std::endl;

	return in_length;
}

// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
Game& Game::GetInstance()
{
	static Game g;
	return g;
}
