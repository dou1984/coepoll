#ifndef __THDID_H__
#define __THDID_H__
#include <atomic>

namespace ashan
{
#define g_thdid thdid::instance().data()

	class thdid final
	{
		static std::atomic_int idx;
		int m_index = 0;

	public:
		thdid();
		static thdid &instance();
		int max() const { return idx; }
		int data() const { return m_index; }		
	};
}
#endif
