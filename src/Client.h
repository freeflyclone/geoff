#ifndef CLIENT_H
#define CLIENT_H

class Client
{
public:
	Client(uint32_t sessionID, bool isLittleEndian, uint16_t clientAppVersion);
	~Client();

private:
	uint32_t m_sessionID;
	bool m_isLittleEndian;
	uint16_t m_clientAppVersion;
};

#endif // CLIENT_H
