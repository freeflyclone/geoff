#include "Game.h"

Game::Game()
{

	srand(12345);

	std::cout << "geoff server ver " << gameAppVersion << " running\n";
}

Game::~Game()
{
}

AppBuffer* Game::commsHandler(websocket::stream<beast::tcp_stream>* ws_, AppBuffer* rxBuffer)
{
	std::cout << __FILE__ << ":" << __LINE__ << std::endl;

	return (nullptr);
}
