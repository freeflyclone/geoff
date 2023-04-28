#include "geoff.h"
#include "Client.h"

Client::Client(uint32_t sessionID, bool isLittleEndian, uint16_t clientAppVersion) :
	m_sessionID(sessionID),
	m_isLittleEndian(isLittleEndian),
	m_clientAppVersion(clientAppVersion)
{
	std::cout << __FUNCTION__ << "(" << __LINE__ << ") : " << m_sessionID << std::endl;
}

Client::~Client()
{
	std::cout << __FUNCTION__ << "(" << __LINE__ << ") : " << m_sessionID << std::endl;
}

uint32_t Client::session_id()
{
	return m_sessionID;
}

ClientManager::ClientManager()
	: m_clients()
{
}

ClientManager::~ClientManager()
{
}

void ClientManager::add_client(uint32_t sessionID, bool isLittleEndian, uint16_t clientAppVersion)
{
	const std::lock_guard<std::mutex> lock(m_clients_mutex);

	m_clients.push_back(std::make_shared<Client>(sessionID, isLittleEndian, clientAppVersion));
}

void ClientManager::delete_client_by_session(uint32_t sessionID)
{
	const std::lock_guard<std::mutex> lock(m_clients_mutex);

	auto client = find_client_by_session(sessionID);

	if (client)
		m_clients.remove(client);
}

std::shared_ptr<Client> ClientManager::find_client_by_session(uint32_t sessionID)
{
	for (auto client : m_clients)
	{
		if (client->session_id() == sessionID)
			return client;
	}

	return nullptr;
}
