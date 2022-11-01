#ifndef __COEPOLLFLAGS_H__
#define __COEPOLLFLAGS_H__
#include <bitset>
#include <vector>
#include "coLock.h"

#define g_epflags coEpollFlags::at()

namespace ashan
{
	class coEpollFlags final
	{
	public:
		enum E_FLAGS
		{
			e_syncothers,
			e_end,
		};
		using coFlag = std::bitset<e_end>;
		void set(int __fd, E_FLAGS __flag, int __val = 1);
		bool exist(int __fd, E_FLAGS __flag);
		coFlag &__at(int __fd);

		static coEpollFlags *at();

	private:
		std::vector<coFlag> m_set;
		coLock m_mtx;
	};

}
#endif