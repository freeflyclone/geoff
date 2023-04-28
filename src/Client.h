#ifndef CLIENT_H
#define CLIENT_H

class Client
{
public:
	Client(uint32_t sessionID, bool isLittleEndian, uint16_t clientAppVersion);
	~Client();

	uint32_t session_id();

private:
	uint32_t m_sessionID;
	bool m_isLittleEndian;
	uint16_t m_clientAppVersion;
};

class ClientManager
{
public:
	ClientManager();
	~ClientManager();

	void add_client(uint32_t sessionID, bool isLittleEndian, uint16_t clientAppVersion);

	std::shared_ptr<Client> find_client_by_session(uint32_t sessionID);
	void delete_client_by_session(uint32_t sessionID);

private:
	std::mutex m_clients_mutex;
	std::list<std::shared_ptr<Client>> m_clients;
};

#endif // CLIENT_H
