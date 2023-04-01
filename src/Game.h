#ifndef GAME_H
#define GAME_H

#include "AppBuffer.h"

#define gameAppVersion 2

class Game
{
public:

	Game();
	~Game();

	AppBuffer* commsHandler(websocket::stream<beast::tcp_stream>* ws_, AppBuffer* rxBuffer);

};

#endif