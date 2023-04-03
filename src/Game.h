#ifndef GAME_H
#define GAME_H

#include "AppBuffer.h"

#define gameAppVersion 2

class Game
{
public:
	static Game& GetInstance();

	std::size_t CommsHandler(beast::flat_buffer buffer, std::size_t bytes_transferred);

private:
	// I know what you're thinking: WTF is this?  It's Magic Statics!
	// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
	// 
	// I tried to figure out how to pass a Game object into the Beast code, and fully face-planted.
	// So I said fuckit, I'm using a Singleton.
	Game();
	~Game();

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;
};

#endif