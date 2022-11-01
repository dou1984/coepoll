#include "coSpinlock.h"
extern "C"
{
#include "hook.h"
}

namespace ashan
{
	const int count_max = 50;
	const int usleep_max = 50;
	coSpinlock::coSpinlock() : state{false}
	{
	}
	void coSpinlock::lock()
	{
		int count = 0;
		while (state.exchange(true, std::memory_order_acquire))
		{
			if (count++ > count_max)
			{
				real_usleep(usleep_max);
			}
		}
	}
	bool coSpinlock::try_lock()
	{
		return !state.exchange(true, std::memory_order_acquire);
	}
	void coSpinlock::unlock()
	{
		int count = 0;
		if (!state.exchange(false, std::memory_order_release))
		{
			if (count++ > count_max)
			{
				real_usleep(usleep_max);
			}
		}
	}
}