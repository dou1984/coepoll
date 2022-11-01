#ifndef __COSPINLOCK_H__
#define __COSPINLOCK_H__
#include <atomic>
#include <mutex>

namespace ashan
{
	class coSpinlock final
	{
		std::atomic_bool state;
	public:
		coSpinlock();
		void lock();
		bool try_lock();
		void unlock();
		//bool is_lock();
	};
}
#endif