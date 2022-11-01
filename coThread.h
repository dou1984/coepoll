#ifndef __COTHREAD_H__
#define __COTHREAD_H__
#include <functional>
#include <thread>
#include <atomic>
#include "coChannel.h"

namespace ashan
{
	class coThread final
	{
		using func = std::function<void()>;
		std::thread m_thd;
		coChannel<func> m_channel;
		std::atomic_int m_isrunning = {0};

	public:
		coThread();
		void run();
		void detach() { m_thd.detach(); }
		void join() { m_thd.join(); }
		bool isrunning() { return m_isrunning == 1; }

		void call(const func &);
		static size_t max();
		static coThread *at(size_t);
	};
}
#endif