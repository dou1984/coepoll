#ifndef __COEPOLL_H__
#define __COEPOLL_H__
#include <memory>
#include <functional>

namespace ashan
{
#define co ashan::coroutine() =
#define co_rand ashan::coroutine() -
#define co_all ashan::coroutine() +
#define co_main(F) ashan::coroutine().main(F)

	int co_setactive(int __fd, int __active);

	class coContext;
	class coroutine final
	{
	public:
		coroutine() = default;
		void operator=(std::function<void()> &&_f);
		void operator-(std::function<void()> &&_f);
		void operator+(const std::function<void()> &_f);

		int main(std::function<void()> &&_f);
	};
}

#endif