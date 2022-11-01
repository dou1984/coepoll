#include "coEpoll.h"
#include "coContext.h"
#include "coCommon.h"
#include "coThread.h"
#include "coEpollNode.h"
#include "coTrigger.h"

namespace ashan
{
	int co_setactive(int __fd, int __active)
	{
		auto n = coEpollNode::find(__fd);
		if (__active != 0)
		{
			n->set_flags(coEpollEvent::e_actived);
		}
		else
		{
			n->set_flags(coEpollEvent::e_actived, 0);
			n->del_epoll_event(__fd);
		}
		return 0;
	}
	void coroutine::operator=(std::function<void()> &&_f)
	{
		coContext::create(std::move(_f));
	}
	void coroutine::operator+(const std::function<void()> &_f)
	{
		auto thd_max = coThread::max();
		for (int i = 0; i < thd_max; i++)
		{
			auto thd = coThread::at(i);
			if (thd != nullptr)
			{
				CO_DBG("corotine operator+ call %d", i);
				thd->call(_f);
			}
		}
	}
	void coroutine::operator-(std::function<void()> &&_f)
	{
		static int seed = 0;
		auto thd_max = coThread::max();
		srand(time(NULL) + seed++);
		int i = rand() % thd_max;
		CO_DBG("coThread call:%d max:%ld", i, thd_max);
		auto thd = coThread::at(i);
		if (thd != nullptr)
		{
			thd->call(_f);
		}
	}
	int coroutine::main(std::function<void()> &&_f)
	{
		ctx_start(std::move(_f));
		return 0;
	}
}
