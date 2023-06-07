#ifndef CONSTS_H
#define CONSTS_H

namespace asteroids
{
	// Take from asteroids.js
	const double FPS = 60;
	const float FRICTION = 0.7f;
	const double SHIP_BLINK_DUR = 0.2f;       // duration in seconds of a single blink during ship's invulnerablility
	const float SHIP_EXPLODE_DUR = 0.5f;     // duration of the ship's explosion in seconds
	const float SHIP_INV_DUR = 3.0f;         // duration of the ship's invulnerablility in seconds
	const int SHIP_SIZE = 20;                // ship height in pixels
	const int SHIP_THRUST = 10;              // acceleration of the ship in pixels per second per second
	const int SHIP_TURN_SPEED = 360;         // turn speed in degrees per second
	const double SHIP_MAX_DELTA_V = 20.0;	 // limit max velocity, pixels per tic.
	const int GUN_MAG_SIZE = 6;				 // max in-flight bullets at once
	const float MUZZLE_VELOCITY = 500;		 // pixels per second
	const float BULLET_DURATION = 1.5;		 // bullet duration in seconds
	const double ROCK_RADIUS = 32.0;         // Rock radius in pixels.
	const int ROCK_SPEED = 50;               // Rock speed in pixels/second;

	// ship rotation increment per timer tick
	const double SHIP_RADS_PER_TICK = SHIP_TURN_SPEED / 180  * 3.1415 / FPS;

	const double FP_4_12 = 4096.0;			 // scale angle to fixed point 4.12 precision
	const double DEGREES_TO_RADS = M_PI / 180;
	const double VIEWPORT_MARGIN = 200;		  // in pixels: for sliding viewport activation
	const int NUM_STARS = 1600;
	const int NUM_LIVES = 5;
}

#endif
