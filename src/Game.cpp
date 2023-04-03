#include "Game.h"

Game::Game()
{

	srand(12345);

	std::cout << "geoff server ver " << gameAppVersion << " running\n";
}

Game::~Game()
{
}

std::size_t Game::CommsHandler(beast::flat_buffer buffer, std::size_t bytes_transferred)
{
	std::cout << "bytes_transferred: " << bytes_transferred << ", buffer.size(): " << buffer.size() << std::endl;

	return bytes_transferred;
}

// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
Game& Game::GetInstance()
{
	static Game g;
	return g;
}
