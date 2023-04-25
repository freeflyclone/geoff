#ifndef CLIENT_H
#define CLIENT_H

class Client
{
public:
	Client(uint32_t clientID, bool isLittleEndian);
	~Client();

private:
	uint32_t m_clientID;
	bool m_isLittleEndian;
};

#endif // CLIENT_H
