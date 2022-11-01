#ifndef __COCHANNEL_H__
#define __COCHANNEL_H__
#include <list>
#include <memory>
#include "coLock.h"
#include "coTrigger.h"

namespace ashan
{
	extern int ctx_suspend_mtx(std::list<std::shared_ptr<coContext>> &_list_ctx, coLock &_mtx);
	class coContext;
	template <class _Type>
	class coChannel
	{
		std::list<std::shared_ptr<coContext>> m_list;
		std::list<_Type> m_data;
		coLock m_mtx;

	public:
		void operator<<(const _Type &o)
		{
			m_mtx.lock();
			m_data.push_back(o);
			if (m_list.empty())
			{
				m_mtx.unlock();
			}
			else
			{
				auto _ctx = m_list.front();
				m_list.pop_front();
				m_mtx.unlock();
				coTrigger::on(_ctx);
			}
		}
		void operator>>(_Type &o)
		{
			m_mtx.lock();
			if (m_data.empty())
			{
			__retry__:
				ctx_suspend_mtx(m_list, m_mtx);
				m_mtx.lock();
			}
			if (m_data.empty())
			{
				goto __retry__;
			}
			o = m_data.front();
			m_data.pop_front();
			m_mtx.unlock();
		}
	};
}

#endif