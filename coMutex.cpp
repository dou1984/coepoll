#include "coMutex.h"
#include "coCommon.h"
#include "coContext.h"
#include "coTrigger.h"

namespace ashan
{
	const int CO_UNLOCK = 0;
	const int CO_LOCK = 1;
	bool coMutex::try_lock()
	{
		int _Val = CO_UNLOCK;
		return m_flag.compare_exchange_weak(_Val, CO_LOCK, std::memory_order_acquire, std::memory_order_acq_rel);
	}
	void coMutex::lock()
	{
		int _Count = m_spin_count;
		int _Val;
		while (_Count--)
		{
			_Val = CO_UNLOCK;
			if (m_flag.compare_exchange_weak(_Val, CO_LOCK, std::memory_order_acquire, std::memory_order_acq_rel))
				return;
		}
		m_mtx.lock();
		_Val = CO_UNLOCK;
		if (m_flag.compare_exchange_weak(_Val, CO_LOCK, std::memory_order_acquire, std::memory_order_acq_rel))
		{
			m_mtx.unlock();
			return;
		}
		ctx_suspend_mtx(m_list_ctx, m_mtx);
	}
	void coMutex::unlock()
	{
		m_mtx.lock();
		if (m_list_ctx.empty())
		{
			int _Val = CO_LOCK;
			m_flag.compare_exchange_weak(_Val, CO_UNLOCK, std::memory_order_acquire, std::memory_order_acq_rel);
			m_mtx.unlock();
		}
		else
		{
			auto _ctx = m_list_ctx.front();
			m_list_ctx.pop_front();
			m_mtx.unlock();
			coTrigger::on(_ctx);
		}
	}
}