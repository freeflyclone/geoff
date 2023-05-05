#ifndef WEBSOCK_SESSION_MANAGER_H
#define WEBSOCK_SESSION_MANAGER_H

template<class SESSION_TYPE>
class WebsockSessionManager
{
public:
	WebsockSessionManager() :
		m_session_id(0),
		m_sessions()
	{
		TRACE("");
	}

	virtual ~WebsockSessionManager()
	{
		TRACE("");
	}

	virtual uint32_t add_session() 
	{
		const std::lock_guard<std::mutex> lock(m_sessions_mutex);
		//TRACE("");

		auto session_id = m_session_id;

		m_sessions.push_back(std::make_shared<SESSION_TYPE>(session_id));

		m_session_id = (m_session_id + 1) & 0xFFFFFFFF;

		return session_id;
	}

	std::shared_ptr<SESSION_TYPE> find_by_id(uint32_t sessionID)
	{
		for (auto session : m_sessions)
		{
			if (session->SessionID() == sessionID)
			{
				//TRACE("session id: " << sessionID);
				return session;
			}
		}

		return nullptr;
	}

	void delete_by_id(uint32_t sessionID)
	{
		const std::lock_guard<std::mutex> lock(m_sessions_mutex);
		//TRACE("");

		auto session = find_by_id(sessionID);

		if (session)
		{
			//TRACE("session id: " << sessionID);
			m_sessions.remove(session);
		}
	}


private:
	std::mutex m_sessions_mutex;
	uint32_t m_session_id;
	std::list<std::shared_ptr<SESSION_TYPE>> m_sessions;
};


#endif // WEBSOCK_SESSION_MANAGER_H