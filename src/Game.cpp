#include "Game.h"

Game::Game()
{

	srand(12345);

	std::cout << "geoff server ver " << gameAppVersion << " running\n";
}

Game::~Game()
{
}

AppBuffer* Game::CommsHandler(net::any_io_executor ex, beast::flat_buffer buffer)
{
	std::cout << __FILE__ << ":" << __LINE__ << std::endl;

	return (nullptr);
}

// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
Game& Game::GetInstance()
{
	static Game g;
	return g;
}
