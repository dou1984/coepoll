#ifndef __SERVPOOL_H__
#define __SERVPOOL_H__
#include <string>
#include <thread>
#include <mutex>
#include "Common.h"
#include "coThread.h"

namespace ashan
{
	template <class _Srv>
	class ServPool
	{
		const std::string m_url;
		std::shared_ptr<_Srv> m_srv;
		int m_fd = INVALID;

	public:
		ServPool(const std::string &_url) : m_url(_url)
		{
			m_srv = std::make_shared<_Srv>();
			init();
		}
		void init()
		{
			if (m_fd != INVALID)
			{
			}
			else if ((m_fd = m_srv->Bind(m_url)) != INVALID)
			{
			}
		}
		void run()
		{
			while (m_srv->Accept() == 0)
			{
			}
		}
		coThread create()
		{
			return coThread(std::bind(&ServPool<_Srv>::run, this));
		}
	};
}

#endif
