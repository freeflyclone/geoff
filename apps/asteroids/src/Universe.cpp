#include "geoff.h"

#include "Universe.h"
#include "Session.h"

using namespace as2;

#undef UN_TRACE
#define UN_TRACE TRACE

Universe::Universe(int width, int height)
	: Size({ static_cast<double>(width), static_cast<double>(height) })
{
	UN_TRACE(__FUNCTION__);
}

Universe::~Universe()
{
	UN_TRACE(__FUNCTION__);
}

void Universe::TickEvent(Session& session)
{
	UN_TRACE(__FUNCTION__);
}

void Universe::PerSessionTickEvent(Session& session)
{
	UN_TRACE(__FUNCTION__);
}