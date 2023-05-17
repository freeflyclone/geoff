#ifndef CONSTS_H
#define CONSTS_H

namespace asteroids
{
	// Take from asteroids.js
	const int FPS = 60;
	const float FRICTION = 0.7f;
	const float SHIP_BLINK_DUR = 0.2f;       // duration in seconds of a single blink during ship's invisibility
	const float SHIP_EXPLODE_DUR = 0.5f;     // duration of the ship's explosion in seconds
	const float SHIP_INV_DUR = 3.0f;         // duration of the ship's invisibility in seconds
	const int SHIP_SIZE = 20;                // ship height in pixels
	const int SHIP_THRUST = 10;              // acceleration of the ship in pixels per second per second
	const int SHIP_TURN_SPEED = 360;         // turn speed in degrees per second
	const float MUZZLE_VELOCITY = 500;		 // pixels per second
	const double ROCK_RADIUS = 32.0;         // Rock radius in pixels.
	const int ROCK_SPEED = 50;               // Rock speed in pixels/second;
	const double SHIP_RADS_PER_TICK = 
				 SHIP_TURN_SPEED / 180 
				 * 3.1415 / FPS;			 // ship rotation increment per timer tick
	const double FP_4_12 = 4096.0;			 // scale angle to fixed point 4.12 precision
	const double DEGREES_TO_RADS = M_PI / 180;
}

#endif
