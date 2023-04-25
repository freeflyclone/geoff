#include "geoff.h"
#include "Client.h"

Client::Client(uint16_t clientID, bool isLittleEndian) :
	m_clientID(clientID),
	m_isLittleEndian(isLittleEndian)
{
}

Client::~Client()
{
}
