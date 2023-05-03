#include "WebsockServer.h"

#include <ios>
#include <iostream>

WebsockServer::WebsockServer() :
	m_serverMutex(),
	m_sessions()
{
	srand(12345);

	std::cout << "geoff server ver " << gameAppVersion << " running\n";
}

WebsockServer::~WebsockServer()
{
}

// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
WebsockServer& WebsockServer::GetInstance()
{
	static WebsockServer g;
	return g;
}

void WebsockServer::IoContext(net::io_context* ioc)
{
	m_ioc = ioc;
}

net::io_context* WebsockServer::IoContext()
{
	return m_ioc;
}

void WebsockServer::OnAccept(OnAcceptCallback_t fn)
{
	fn(m_sessions.add_session());
}

void WebsockServer::OnTxReady(OnTxReady_t fn)
{
	m_on_tx_ready_cb = fn;
}

void WebsockServer::OnTxReady()
{
	if (m_on_tx_ready_cb)
	{
		std::unique_ptr<AppBuffer> txBuff;

		if (GetNextTxBuffer(txBuff))
			m_on_tx_ready_cb(txBuff->data(), txBuff->bytesWritten());
	}
}
void WebsockServer::OnClose(uint32_t sessionID)
{
	m_sessions.delete_by_id(sessionID);
}

void WebsockServer::CommsHandler(uint32_t sessionID, beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::recursive_mutex> lock(m_serverMutex);

	auto session = m_sessions.find_by_id(sessionID);

	if (!session)
		return;

	uint8_t* buff = (uint8_t*)static_cast<net::const_buffer>(in_buffer.data()).data();

	session->CommsHandler(buff, in_length);
}

void WebsockServer::CommitTxBuffer(std::unique_ptr<AppBuffer> & buffer)
{
	const std::lock_guard<std::recursive_mutex> lock(m_serverMutex);

	m_txQue.push_back(std::move(buffer));
}

bool WebsockServer::GetNextTxBuffer(std::unique_ptr<AppBuffer> & buff)
{
	const std::lock_guard<std::recursive_mutex> lock(m_serverMutex);

	if (m_txQue.empty())
		return false;

	// Get next AppBuffer from TX que
	buff = std::move(m_txQue.front());
	m_txQue.pop_front();

	return true;
}
