#ifndef CLIENT_H
#define CLIENT_H

class Client
{
public:
	Client(uint16_t clientID, bool isLittleEndian);
	~Client();

private:
	uint16_t m_clientID;
	bool m_isLittleEndian;
};

#endif // CLIENT_H
