#include "geoff.h"
#include "Client.h"

Client::Client(uint32_t sessionID, bool isLittleEndian, uint16_t clientAppVersion) :
	m_sessionID(sessionID),
	m_isLittleEndian(isLittleEndian),
	m_clientAppVersion(clientAppVersion)
{
}

Client::~Client()
{
}
