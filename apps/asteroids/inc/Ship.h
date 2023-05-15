#ifndef SHIP_H
#define SHIP_H

#include "Structs.h"

#define SH_TRACE(...)

class Ship2 : public Context, public Position, public Orientation, public Velocity
{
public:
	Ship2(uint16_t cw, uint16_t ch, double dx, double dy, double angle);
	~Ship2();

	void GetXYA(double& x, double& y, double& a);

	void MoveShip();
	void FireShot();

	void KeyEvent(int key, bool isDown);
	void TickEvent();

private:
};
#endif
