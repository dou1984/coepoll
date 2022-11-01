#include "coEpollfd.h"
#include "coEpollNode.h"
extern "C"
{
#include "hook.h"
}
namespace ashan
{
	static const long long g_msec = 1000;
	static thread_local std::vector<std::shared_ptr<coEpollfd>> g_epollfd;
	coEpollfd::coEpollfd()
	{
		m_fd = real_timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
		if (m_fd < 0)
		{
			CO_DBG("timerfd_create errno:%d errorinfo:%s", errno, strerror(errno));
		}
	}
	coEpollfd::~coEpollfd()
	{
		auto n = coEpollNode::find(m_fd);
		n->del_epoll_event(m_fd);
		real_close(m_fd);
	}
	void coEpollfd::set(struct epoll_event *__event, int __maxevents, int __size)
	{
		m_event = __event;
		m_maxevents = __maxevents;
		m_size = __size;
	}
	void coEpollfd::clear()
	{
		m_event = nullptr;
		m_maxevents = 0;
	}
	int coEpollfd::push_back(int _events, void *_ptr)
	{
		auto &o = m_event[m_size++];
		o.events = _events;
		o.data.ptr = _ptr;
		return m_size;
	}
	int coEpollfd::set_timer(int _stamp)
	{
		auto n = coEpollNode::find(m_fd);
		n->set_flags(coEpollEvent::e_actived);
		struct itimerspec t;
		t.it_value.tv_sec = _stamp / g_msec;
		t.it_value.tv_nsec = (_stamp % g_msec) * 1000000;
		//t.it_interval.tv_sec = _stamp / g_msec;
		//t.it_interval.tv_nsec = (_stamp % g_msec) * 1000000;
		int r = timerfd_settime(m_fd, 0, &t, 0);
		if (r < 0)
		{
			CO_DBG("settimer error %d %d", m_fd, _stamp);
		}
		n->set_epoll_event(m_fd, EPOLLIN);
		return r;
	}
	int coEpollfd::stop_timer()
	{
		auto n = coEpollNode::find(m_fd);
		if (n->events & EPOLLIN)
		{
			itimerspec t;
			real_read(m_fd, &t, sizeof(t));
		}
		n->mod_epoll_event(m_fd, 0);
		return 0;
	}
	std::shared_ptr<coEpollfd> coEpollfd::at(int __fd)
	{
		assert(__fd != INVALID);
		if (__fd < g_epollfd.size())
		{
			return g_epollfd[__fd];
		}
		return nullptr;
	}
	std::shared_ptr<coEpollfd> coEpollfd::create()
	{
		auto epfd = std::make_shared<coEpollfd>();
		int __fd = epfd->get_fd();
		if (__fd >= g_epollfd.size())
		{
			g_epollfd.resize(__fd + 1);
		}
		g_epollfd[__fd] = epfd;
		return epfd;
	}
	void coEpollfd::clear(int __fd)
	{
		assert(__fd != INVALID);
		assert(__fd < g_epollfd.size());
		g_epollfd[__fd] = nullptr;
	}
}
