#include "coEpollEvent.h"
#include "coFunc.h"
#include "coContext.h"
#include "coCommon.h"
#include "coEpollFlags.h"
#include "coEpollNode.h"
#include "thdid.h"

namespace ashan
{
	void coEpollEvent::set(int __epfd, int fd, struct epoll_event *ev)
	{
		epfd = __epfd;
		_src.events = ev->events;
		_src.data.ptr = ev->data.ptr;
		_dst.events = ev->events;
		_dst.data.fd = fd;
	}
	void coEpollEvent::set(int __epfd, struct epoll_event *ev)
	{
		epfd = __epfd;
		_dst.events = ev->events;
		_dst.data.fd = ev->data.fd;
	}
	void coEpollEvent::reset()
	{
		epfd = INVALID;
		_src.events = 0;
		_src.data.ptr = 0;
		_dst.events = 0;
		_dst.data.ptr = 0;
		events = 0;
		flags.reset();
	}
	void coEpollEvent::set_noblock(int fd)
	{
		if (flags[coEpollEvent::e_async])
			return;
		set_flags(coEpollEvent::e_async);
		if (real_fcntl(fd, F_SETFL, O_NONBLOCK | real_fcntl(fd, F_GETFL)) == INVALID)
		{
			CO_DBG("fcntl error fd:%d err:%d", fd, errno);
		}
	}
	void coEpollEvent::mod_epoll_event(int fd, int __events)
	{
		if (__events == _dst.events)
			return;
		struct epoll_event ev;
		ev.events = __events;
		ev.data.fd = fd;
		set(get_epfd(), &ev);
		CO_DBG("epoll_ctl_mod epfd:%d fd:%d events:%#x", get_epfd(), fd, ev.events);
		if (real_epoll_ctl(get_epfd(), EPOLL_CTL_MOD, fd, &_dst) == INVALID)
		{
			CO_DBG("epoll_ctl_mod epfd:%d fd:%d err:%d %s", get_epfd(), fd, errno, strerror(errno));
		}
	}
	void coEpollEvent::set_epoll_event(int fd, int __events)
	{
		if (flags[coEpollEvent::e_settled])
			return;
		set_flags(coEpollEvent::e_settled);
		struct epoll_event ev;
		ev.events = __events;
		ev.data.fd = fd;
		set(get_epfd(), &ev);
		CO_DBG("epoll_ctl_add epfd:%d thdid:%d fd:%d events:%#x", get_epfd(), g_thdid, fd, ev.events);
		if (real_epoll_ctl(get_epfd(), EPOLL_CTL_ADD, fd, &_dst) == INVALID)
		{
			CO_DBG("epoll_ctl_add epfd:%d fd:%d err:%d", get_epfd(), fd, errno);
		}
	}
	void coEpollEvent::del_epoll_event(int fd)
	{
		if (!flags[coEpollEvent::e_settled])
			return;
		set_flags(coEpollEvent::e_settled, 0);
		reset();
		CO_DBG("epoll_ctl_del epfd:%d fd:%d", get_epfd(), fd);
		if (real_epoll_ctl(get_epfd(), EPOLL_CTL_DEL, fd, &_dst) == INVALID)
		{
			CO_DBG("epoll_ctl_del epfd:%d fd:%d err:%d", get_epfd(), fd, errno);
		}
	}
	int coEpollEvent::ctl_epoll_event(int __epfd, int __op, int __fd, struct epoll_event *__event)
	{
		set(__epfd, __fd, __event);
		int r = 0;
		if ((r = real_epoll_ctl(get_epfd(), __op, __fd, &_dst)) == INVALID)
		{
			CO_DBG("real epoll_ctl error fd:%d error:%d", __fd, errno);
			return r;
		}
		if (__op == EPOLL_CTL_ADD)
		{
			set_flags(coEpollEvent::e_settled);
		}
		else if (__op == EPOLL_CTL_DEL)
		{
			set_flags(coEpollEvent::e_settled, 0);
			reset();
		}
		else if (__op == EPOLL_CTL_MOD)
		{
		}
		return r;
	}

	int coEpollEvent::resume(int __flags)
	{
		if (__autoread() == INVALID)
		{
			return 0;
		}
		auto &_list =
			flags[e_epoll_rdwr]			   ? incoro
			: (__flags == e_suspend_write) ? outcoro
										   : incoro;
		if (_list.empty())
		{
			CO_ERR("resume error fd:%d events:%#x is:%s in:%ld out:%ld", _dst.data.fd,
				   events, __flags == e_suspend_write ? "out" : "in", incoro.size(), outcoro.size());
			set_flags(__flags == e_suspend_write ? e_ioctl_write : e_ioctl_read, 0);			
			return 0;
		}
		CO_DBG("resume fatal fd:%d events:%#x is:%s in:%ld out:%ld", _dst.data.fd,
			   events, __flags == e_suspend_write ? "out" : "in", incoro.size(), outcoro.size());
		auto _ctx = _list.front();
		_list.pop_front();
		_ctx->resume();
		return 0;
	}
	int coEpollEvent::suspend(int __flags, bool __retry)
	{
		if (__flags == e_suspend_rw)
			set_flags(e_epoll_rdwr);
		auto &_list = flags[e_suspend_rw]		   ? incoro
					  : __flags == e_suspend_write ? outcoro
												   : incoro;
		int r = ctx_suspend(_list, __retry);
		if (__flags == e_suspend_rw)
			set_flags(e_epoll_rdwr, 0);
		return r;
	}

	int coEpollEvent::__resume_trigger()
	{
		if (flags[e_threadsafe])
		{
			auto _node = coEpollNode::at();
			if (_node != nullptr)
			{
				_node->resume_trigger();
			}
			return 1;
		}
		return 0;
	}
	int coEpollEvent::__autoread()
	{
		if (flags[e_autoread])
		{
			int fd = _dst.data.fd;
			unsigned long long reserve = 0;
			int r = real_read(fd, &reserve, sizeof(reserve));
			if (r == INVALID)
			{
				assert(EAGAIN);
				CO_DBG("read fd:%d r:%d error(%d):%s", fd, r, errno, strerror(errno));
				set_flags(e_ioctl_read, 0);
			}
			else
			{
				__resume_trigger();
			}
			return r;
		}
		return 0;
	}
}