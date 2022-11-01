#include <vector>
#include "coLock.h"
#include "coThread.h"
#include "coCommon.h"
#include "Common.h"
#include "thdid.h"

namespace ashan
{
	static std::vector<coThread *> g_thread;
	static coLock g_mutex;
	coThread::coThread() : m_thd(std::bind(&coThread::run, this))
	{
		std::lock_guard<coLock> _lock(g_mutex);
		g_thread.emplace_back(this);
	}
	void coThread::run()
	{
		ctx_start(
			[this]()
			{
				m_isrunning = 1;
				while (true)
				{
					func f;
					m_channel >> f;
					auto c = coContext::create(std::move(f));
				}
			});
	}
	size_t coThread::max()
	{
		std::lock_guard<coLock> _lock(g_mutex);
		return g_thread.size();
	}
	coThread *coThread::at(size_t idx)
	{
		std::lock_guard<coLock> _lock(g_mutex);
		if (idx < g_thread.size())
		{
			return g_thread[idx];
		}
		return nullptr;
	}
	void coThread::call(const func &_f)
	{
		m_channel << _f;
	}
}