#ifndef __COMUTEX_H__
#define __COMUTEX_H__
#include <atomic>
#include <mutex>
#include <list>
#include <memory>
#include "coLock.h"

namespace ashan
{
	class coContext;
	class coMutex final
	{
		std::atomic_int m_flag;
		std::list<std::shared_ptr<coContext>> m_list_ctx;
		coLock m_mtx;
		int m_spin_count = 1; //碰撞概率高、锁时间长的使用1,否则使用25
	public:
		bool try_lock();
		void lock();
		void unlock();
	};
}

#endif