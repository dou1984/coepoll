#include <mutex>
#include "Common.h"
#include "coEpollFlags.h"

namespace ashan
{
	void coEpollFlags::set(int __fd, E_FLAGS __flag, int __val)
	{
		std::lock_guard<coLock> _lock(m_mtx);
		__at(__fd).set(__flag, __val);
	}
	bool coEpollFlags::exist(int __fd, E_FLAGS __flag)
	{
		std::lock_guard<coLock> _lock(m_mtx);
		return __at(__fd)[__flag];
	}
	coEpollFlags::coFlag &coEpollFlags::__at(int __fd)
	{
		assert(__fd >= 0);
		if (__fd >= m_set.size())
		{
			m_set.resize(__fd + 1);
		}
		return m_set[__fd];
	}
	coEpollFlags *coEpollFlags::at()
	{
		static coEpollFlags _this;
		return &_this;
	}
}