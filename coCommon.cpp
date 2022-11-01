#include <stack>
#include <thread>
#include <vector>
#include <unordered_set>
#include "coCommon.h"
#include "coEpollfd.h"
#include "coTrigger.h"
#include "coEpollNode.h"
#include "thdid.h"
extern "C"
{
#include "hook.h"
}

namespace ashan
{
	int init_epoll_create1()
	{
		int __init_hook = init_hook();
		assert(__init_hook);
		int epfd = real_epoll_create1(EPOLL_CLOEXEC);
		if (epfd == INVALID)
		{
			CO_DBG("real_epoll_create1 error %d", errno);
		}
		CO_DBG("thdid:%d epfd:%d", g_thdid, epfd);
		return epfd;
	}

	static thread_local int g_epfd = init_epoll_create1();
	static thread_local std::stack<std::shared_ptr<coContext>> g_last_ctx; //
	static thread_local coTrigger g_trg;
	int co_ee_resume(std::shared_ptr<coEpollEvent> &n, int _fired, int _suspend)
	{
		CO_DBG("co_ee_resume thdid:%d suspend:%d", g_thdid, _suspend);
		n->set_flags(_fired);
		while (n->is_flags(_fired))
			n->resume(_suspend);
		return 0;
	}
	int get_epfd()
	{
		return g_epfd;
	}
	int ctx_suspend(std::list<std::shared_ptr<coContext>> &__list, bool __retry)
	{
		assert(!g_last_ctx.empty());
		auto _ctx = g_last_ctx.top();
		g_last_ctx.pop();
		__retry ? __list.push_front(_ctx) : __list.push_back(_ctx);
		_ctx->suspend();
		g_last_ctx.push(_ctx);
		return 0;
	}
	int ctx_suspend_mtx(std::list<std::shared_ptr<coContext>> &__list, coLock &_mtx)
	{
		assert(!g_last_ctx.empty());
		auto _ctx = g_last_ctx.top();
		g_last_ctx.pop();
		__list.push_back(_ctx);
		_mtx.unlock();

		_ctx->suspend();
		g_last_ctx.push(_ctx);
		return 0;
	}
	void ctx_epoll_wait_resume_epollevent(int events, std::shared_ptr<coEpollEvent> &n)
	{
		n->set_flags(coEpollEvent::e_reenterable);
		if (events & EPOLLOUT)
		{
			n->events = events & EPOLLOUT;
			co_ee_resume(n, coEpollEvent::e_ioctl_write, coEpollEvent::e_suspend_write);
		}
		if (events & EPOLLIN)
		{
			n->events = events & EPOLLIN;
			co_ee_resume(n, coEpollEvent::e_ioctl_read, coEpollEvent::e_suspend_read);
		}
		if (events & (EPOLLERR | EPOLLHUP))
		{
			n->events = events & (EPOLLERR | EPOLLHUP);
			co_ee_resume(n, coEpollEvent::e_ioctl_read, coEpollEvent::e_suspend_read);
			co_ee_resume(n, coEpollEvent::e_ioctl_write, coEpollEvent::e_suspend_write);
		}
		n->set_flags(coEpollEvent::e_reenterable, 0);
	}
	void ctx_epoll_wait_resume_epollfd(std::unordered_set<int> &_set)
	{
		CO_DBG("set size:%ld", _set.size());
		for (auto __fd : _set)
		{
			auto n = coEpollNode::find(__fd);
			assert(n->is_flags(coEpollEvent::e_epollfd));
			n->resume(coEpollEvent::e_epoll_rdwr);
		}
		_set.clear();
	}
	void ctx_epoll_wait()
	{
		std::unordered_set<int> epfdset;
		std::vector<struct epoll_event> e;
		e.resize(MAX_EPOLL_NODE);
		do
		{
			int r = real_epoll_wait(get_epfd(), e.data(), e.size(), INVALID);
			CO_DBG("real_epoll_wait epfd:%d r:%d", get_epfd(), r);
			for (int i = 0; i < r; i++)
			{
				auto &t = e[i];
				auto n = coEpollNode::find(t.data.fd);
				if (n->epfd == get_epfd())
				{
					ctx_epoll_wait_resume_epollevent(t.events, n);
				}
				else
				{
					auto ep = coEpollfd::at(n->epfd);
					ep->push_back(t.events, n->_src.data.ptr);
					if (ep->nofull())
					{
						epfdset.insert(n->epfd);
					}
					else
					{
						epfdset.erase(n->epfd);
						std::unordered_set<int> _set;
						_set.insert(n->epfd);
						ctx_epoll_wait_resume_epollfd(_set);
					}
				}
			}
			ctx_epoll_wait_resume_epollfd(epfdset);
		} while (true);
	}
	int ctx_start(std::function<void()> &&__init)
	{
		assert(get_epfd() != INVALID);
		auto c = coContext::create(std::move(__init));
		auto _waiter = coContext::create(ctx_epoll_wait);
		CO_DBG("ctx_start quit thdid:%d", g_thdid);
		return 0;
	}
	int ctx_pushback(std::shared_ptr<coContext> &_ctx)
	{
		g_last_ctx.push(_ctx);
		return 0;
	}
	int ctx_popback()
	{
		g_last_ctx.pop();
		return 0;
	}
	int ctx_resume(std::shared_ptr<coContext> &_ctx)
	{
		_ctx->resume();
		return 0;
	}
}