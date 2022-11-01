#ifndef __UCODE_H__
#define __UCODE_H__
#include <stdint.h>
#include <vector>

namespace ashan
{
	class ucode
	{
		const int BITCOUNT = 64;
		std::vector<uint64_t> m_code;
		int __geti(int _code) const { return _code / BITCOUNT; }
		int __getj(int _code) const { return _code % BITCOUNT; }
		int __getfirst() const;
		void __set(int i, int j) { m_code[i] &= (1 << j); }
		void __reset(int i, int j) { m_code[i] &= ~(1 << j); }
		void set(int _code);

	public:
		int getfirst();
		int reset(int _f);
	};
}
#endif