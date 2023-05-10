#ifndef WEBSOCK_SESSION_MANAGER_H
#define WEBSOCK_SESSION_MANAGER_H

template<class SESSION_TYPE>
class WebsockSessionManager
{
public:
	typedef std::map<uint32_t, std::shared_ptr<SESSION_TYPE>> SessionMap;

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
		
		auto session_id = m_session_id;

		m_sessions[m_session_id] = std::make_shared<SESSION_TYPE>(m_session_id);
		m_session_id = (m_session_id + 1) & 0xFFFFFFFF;

		return session_id;
	}

	std::shared_ptr<SESSION_TYPE> find_by_id(uint32_t sessionID)
	{
		return m_sessions[sessionID];
	}

	void delete_by_id(uint32_t sessionID)
	{
		const std::lock_guard<std::mutex> lock(m_sessions_mutex);

		m_sessions.erase(sessionID);
	}

	size_t get_count()
	{
		return m_sessions.size();
	}

	SessionMap& get_map()
	{
		return m_sessions;
	}

private:
	std::mutex m_sessions_mutex;
	uint32_t m_session_id;
	SessionMap m_sessions;
};


#endif // WEBSOCK_SESSION_MANAGER_H