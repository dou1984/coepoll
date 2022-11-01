#include "ucode.h"

namespace ashan
{
	void ucode::set(int _code)
	{
		int i = __geti(_code);
		if (i >= m_code.size())
			m_code.resize(i + 1);
		int j = __getj(_code);
		__set(i, j);
	}
	int ucode::__getfirst() const
	{
		for (int i = 0; i < m_code.size(); i++)
		{
			auto _code = m_code[i];
			if (_code == uint64_t(-1))
				continue;
			for (int j = 0; j < BITCOUNT; j++)
			{
				if (_code & (1 << j))
					continue;
				return i * BITCOUNT + j;
			}
		}
		return m_code.size() * BITCOUNT;
	}
	int ucode::getfirst()
	{
		int _f = __getfirst();
		set(_f);
		return _f;
	}
	int ucode::reset(int _f)
	{
		__reset(__geti(_f), __getj(_f));
		return 0;
	}
}