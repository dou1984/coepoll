#include "thdid.h"
#include "comdef.h"

namespace ashan
{
	std::atomic_int thdid::idx(0);

	thdid::thdid()
	{
		m_index = idx++;
		CO_DBG("thdid max:%d", m_index);
	}
	thdid &thdid::instance()
	{
		static thread_local thdid _ins;
		return _ins;
	}
}