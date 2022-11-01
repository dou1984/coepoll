#include <assert.h>
#include "Common.h"
#include "coFunc.h"
#include "coEpollNode.h"
#include "coContext.h"
#include "coEpollFlags.h"
#include "coEpollfd.h"
#include "thdid.h"

using namespace ashan;
static int g_init_hook = init_hook();
static const long long g_usec = 1000000;

static int getsockerr(int fd)
{
	int err = 0;
	socklen_t len = sizeof(socklen_t);
	return getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&err, &len) == INVALID ? INVALID : err;
}
/*
int co_autoactive(std::shared_ptr<coEpollEvent> &n, int __fd, int __active)
{
	if (__active != 0)
	{
		n->set_flags(coEpollEvent::e_actived);
		n->set_flags(coEpollEvent::e_autodisable);
	}
	else if (n->is_flags(coEpollEvent::e_autodisable))
	{
		n->set_flags(coEpollEvent::e_actived, 0);
		n->del_epoll_event(__fd);
	}
	return 0;
}
*/
int co_socket(int __af, int __type, int __protocol)
{
	int fd = real_socket(__af, __type, __protocol);
	if (fd != INVALID)
	{
		auto n = coEpollNode::find(fd);
		n->set_flags(coEpollEvent::e_actived);
	}
	return fd;
}
int co_close(int __fd)
{
	auto n = coEpollNode::find(__fd);
	if (n->is_flags(coEpollEvent::e_epollfd))
	{
		coEpollfd::clear(__fd);
	}
	if (n->is_flags(coEpollEvent::e_actived))
	{
		n->del_epoll_event(__fd);
	}
	return real_close(__fd);
}
ssize_t co_recv_async(int fd, char *buf, int len, int flags)
{
	auto n = coEpollNode::find(fd);
	bool retry = n->is_flags(coEpollEvent::e_reenterable);
	if (retry)
	{
		auto r = real_recv(fd, buf, len, flags);
		if (r > 0)
		{
			CO_DBG("real_recv fd:%d r:%ld", fd, r);
			return r;
		}
		assert(r >= 0);
		n->set_flags(coEpollEvent::e_ioctl_read, 0);
	}
	n->set_noblock(fd);
	n->set_epoll_event(fd, (EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET));
__retry__:
	CO_DBG("recv suppend fd:%d", fd);
	n->suspend(coEpollEvent::e_suspend_read, retry);
	n = coEpollNode::find(fd);
	CO_DBG("recv resume fd:%d events:%x", fd, n->events);
	if (n->events & EPOLLIN)
	{
		auto r = real_recv(fd, buf, len, flags);
		if (r > 0)
		{
			CO_DBG("real_recv fd:%d r:%ld", fd, r);
			return r;
		}
		assert(r >= 0);
		n->set_flags(coEpollEvent::e_ioctl_read, 0);
		goto __retry__;
	}
	if (n->events & (EPOLLERR | EPOLLHUP))
	{
		n->del_epoll_event(fd);
	}
	return INVALID;
}
ssize_t co_recv(int fd, char *buf, int len, int flags)
{
	auto n = coEpollNode::find(fd);
	if (!n->isactived() || n->isctrl())
	{
		return real_recv(fd, buf, len, flags);
	}
	return co_recv_async(fd, buf, len, flags);
}
ssize_t co_read(int fd, void *buf, size_t len)
{
	auto n = coEpollNode::find(fd);
	if (!n->isactived() || n->isctrl())
	{
		return real_read(fd, buf, len);
	}
	return co_recv_async(fd, (char *)buf, len, 0);
}
ssize_t co_recvfrom(int fd, char *buf, int len, int flags, struct sockaddr *from, int *fromlen)
{
	auto n = coEpollNode::find(fd);
	bool retry = n->is_flags(coEpollEvent::e_reenterable);
	if (retry)
	{
		auto r = real_recvfrom(fd, buf, len, flags, from, fromlen);
		if (r > 0)
		{
			CO_DBG("real_recvfrom fd:%d r:%ld", fd, r);
			return r;
		}
		n->set_flags(coEpollEvent::e_ioctl_read, 0);
	}
	n->set_noblock(fd);
	n->set_epoll_event(fd, (EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET));
__retry__:
	n->suspend(coEpollEvent::e_suspend_read, retry);
	n = coEpollNode::find(fd);
	if (n->events & EPOLLIN)
	{
		auto r = real_recvfrom(fd, buf, len, flags, from, fromlen);
		if (r > 0)
		{
			CO_DBG("real_recvfrom fd:%d r:%ld", fd, r);
			return r;
		}
		n->set_flags(coEpollEvent::e_ioctl_read, 0);
		goto __retry__;
	}
	if (n->events & (EPOLLERR | EPOLLHUP))
	{
		n->del_epoll_event(fd);
	}
	return INVALID;
}
ssize_t co_sendto(int fd, const char *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
__retry__:
	auto n = coEpollNode::find(fd);
	int r = real_sendto(fd, buf, len, flags, to, tolen);
	if (r == INVALID)
	{
		if (errno == EAGAIN || errno == EINPROGRESS)
		{
			n->mod_epoll_event(fd, (EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET));
			n->set_flags(coEpollEvent::e_ioctl_write, 0);
			n->suspend(coEpollEvent::e_suspend_write);
			goto __retry__;
		}
		n->del_epoll_event(fd);
		return r;
	}
	n->mod_epoll_event(fd, (EPOLLIN | /*EPOLLOUT |*/ EPOLLERR | EPOLLHUP | EPOLLET));
	return r;
}
ssize_t co_send_async(int fd, const char *buf, int len, int flags)
{
	auto n = coEpollNode::find(fd);
	n->set_noblock(fd);
__retry__:
	int r = real_send(fd, buf, len, flags);
	if (r == INVALID)
	{
		if (errno == EAGAIN || errno == EINPROGRESS)
		{
			n->mod_epoll_event(fd, (EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET));
			n->set_flags(coEpollEvent::e_ioctl_write, 0);
			CO_DBG("send suppend fd:%d", fd);
			n->suspend(coEpollEvent::e_suspend_write);
			CO_DBG("send resume fd:%d", fd);
			n = coEpollNode::find(fd);
			goto __retry__;
		}
		n->del_epoll_event(fd);
		CO_DBG("send error fd:%d error:%d %s", fd, errno, strerror(errno));
		return r;
	}
	CO_DBG("send fd:%d success:%d", fd, r);
	n->mod_epoll_event(fd, (EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET));
	return r;
}
ssize_t co_send(int fd, const char *buf, int len, int flags)
{
	auto n = coEpollNode::find(fd);
	if (!n->isactived() || n->isctrl())
	{
		return real_send(fd, buf, len, flags);
	}
	return co_send_async(fd, buf, len, flags);
}
ssize_t co_write(int fd, const void *buf, size_t len)
{
	auto n = coEpollNode::find(fd);
	if (!n->isactived() || n->isctrl())
	{
		return real_write(fd, buf, len);
	}
	return co_send_async(fd, (const char *)buf, len, 0);
}
int co_connect(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	auto n = coEpollNode::find(fd);
	if (!n->isactived() || n->isctrl())
	{
		return real_connect(fd, addr, addrlen);
	}
	n->set_noblock(fd);
	int c = 0;
	if ((c = real_connect(fd, addr, addrlen)) == INVALID)
	{
		if (errno == EINPROGRESS)
		{			
			n->set_epoll_event(fd, EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP);		
			CO_DBG("connect suspend fd:%d", fd);
			n->suspend(coEpollEvent::e_suspend_rw);
			CO_DBG("connect resume fd:%d", fd);
			n = coEpollNode::find(fd);
			if (n->events & (EPOLLIN | EPOLLOUT))
			{
				int err = 0;
				if ((err = getsockerr(fd)) == 0)
				{
					n->mod_epoll_event(fd, (EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET));
					n->set_flags(coEpollEvent::e_ioctl_read, 0);
					return fd;
				}
				CO_DBG("connect error fd:%d err:%d", fd, err);
			}
			else if (n->events & (EPOLLERR | EPOLLHUP))
			{
				CO_DBG("connect error fd:%d err:%d", fd, errno);
			}
			n->del_epoll_event(fd);
			n->set_flags(coEpollEvent::e_ioctl_read, 0);
		}
		return INVALID;
	}
	return c;
}
int co_accept(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
	auto n = coEpollNode::find(fd);
	if (n->isctrl() || !n->isactived())
	{
		return real_accept(fd, addr, addrlen);
	}
	n->set_noblock(fd);
	n->set_epoll_event(fd, EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP);
	n->suspend(coEpollEvent::e_suspend_rw);
	n = coEpollNode::find(fd);
	if (n->events & (EPOLLIN | EPOLLOUT))
	{
		int r = real_accept(fd, addr, addrlen);
		if (r != INVALID)
		{
			auto _acceptor = coEpollNode::find(r);
			assert(!_acceptor->flags[coEpollEvent::e_actived]);
			_acceptor->set_epoll_event(r, (EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET));
			_acceptor->set_flags(coEpollEvent::e_actived);
		}
		n->set_flags(coEpollEvent::e_ioctl_read, 0);
		return r;
	}
	if (n->events & (EPOLLERR | EPOLLHUP))
	{
		CO_DBG("accept error fd:%d err:%d", fd, errno);
		n->del_epoll_event(fd);
	}
	n->set_flags(coEpollEvent::e_ioctl_read, 0);
	return INVALID;
}
int co_fcntl(int __fd, int __cmd, ...)
{
	if (__cmd == F_SETFL)
	{
		EXPAND_PARAM(__cmd);
		size_t __flag = (size_t)(__arg);
		auto n = coEpollNode::find(__fd);
		n->set_flags(coEpollEvent::e_ctrl_async, __flag & O_NONBLOCK);
		return real_fcntl(__fd, __cmd, __arg);
	}
	EXPAND_PARAM(__cmd);
	return real_fcntl(__fd, __cmd, __arg);
}
int co_sleep_base(long long timer)
{
	int fd = real_timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if (fd < 0)
	{
		CO_DBG("timerfd_create errno:%d errorinfo:%s", errno, strerror(errno));
		return 0;
	}
	CO_DBG("co_sleep_base begin thdid:%d fd:%d %lld", g_thdid, fd, timer);
	auto n = coEpollNode::find(fd);
	n->set_flags(coEpollEvent::e_actived);
	struct itimerspec t;
	t.it_value.tv_sec = timer / g_usec;
	t.it_value.tv_nsec = (timer % g_usec) * 1000000;
	t.it_interval.tv_sec = timer / g_usec;
	t.it_interval.tv_nsec = (timer % g_usec) * 1000000;
	int r = timerfd_settime(fd, 0, &t, 0);
	if (r < 0)
	{
		CO_DBG("timerfd_settime errno:%d errorinfo:%s", errno, strerror(errno));
		goto __error__;
	}
	n->set_epoll_event(fd, EPOLLIN);
	n->suspend(coEpollEvent::e_suspend_read);
	n = coEpollNode::find(fd);
	if (n->events & EPOLLIN)
	{
		itimerspec t;
		real_read(fd, &t, sizeof(t));
	}
__error__:
	n->del_epoll_event(fd);
	n->set_flags(coEpollEvent::e_ioctl_read, 0);
	real_close(fd);
	CO_DBG("co_sleep_base end fd:%d %lld", fd, timer);
	return 0;
}
unsigned int co_sleep(unsigned int t)
{
	return co_sleep_base(t * g_usec);
}
int co_usleep(useconds_t t)
{
	return co_sleep_base(t);
}
int co_epoll_create_base()
{
	auto ep = coEpollfd::create();
	int __epfd = ep->get_fd();
	auto n = coEpollNode::find(__epfd);
	n->set_flags(coEpollEvent::e_actived);
	n->set_flags(coEpollEvent::e_epollfd);
	CO_DBG("epoll_create epfd:%d", __epfd);
	return __epfd;
}
int co_epoll_create(int __size)
{
	return co_epoll_create_base();
}
int co_epoll_create1(int __flag)
{
	return co_epoll_create_base();
}
int co_is_epollfd(int __epfd)
{
	auto ep = coEpollNode::find(__epfd);
	if (!ep->is_flags(coEpollEvent::e_epollfd))
	{
		CO_DBG("%d isn't epollfd", __epfd);
		return INVALID;
	}
	return 0;
}
int co_epoll_ctl(int __epfd, int __op, int __fd, struct epoll_event *__event)
{
	if (co_is_epollfd(__epfd) == INVALID)
	{
		return real_epoll_ctl(__epfd, __op, __fd, __event);
	}
	auto n = coEpollNode::find(__fd);
	return n->ctl_epoll_event(__epfd, __op, __fd, __event);
}
int co_epoll_wait(int __epfd, struct epoll_event *__event, int __maxevents, int __timeout)
{
	if (co_is_epollfd(__epfd) == INVALID)
	{
		return real_epoll_wait(__epfd, __event, __maxevents, __timeout);
	}
	auto ep = coEpollfd::at(__epfd);
	ep->set(__event, __maxevents, 0);
	if (__timeout > 0)
	{
		ep->set_timer(__timeout);
	}
	auto n = coEpollNode::find(__epfd);
	n->suspend(coEpollEvent::e_suspend_rw, false);
	ep = coEpollfd::at(__epfd);
	ep->clear();
	if (__timeout > 0)
	{
		ep->stop_timer();
	}
	return ep->size();
}
int co_epoll_pwait(int __epfd, struct epoll_event *__event, int __maxevents, int __timeout, const __sigset_t *__ss)
{
	assert(false);
	return real_epoll_pwait(__epfd, __event, __maxevents, __timeout, __ss);
}
int co_eventfd(unsigned int __initval, int __flags)
{
	auto __fd = real_eventfd(__initval, __flags);
	if (__fd != INVALID)
	{
		auto n = coEpollNode::find(__fd);
		n->set_flags(coEpollEvent::e_actived);
		n->set_flags(coEpollEvent::e_ctrl_async);
	}
	return __fd;
}
int co_timerfd_create(int __clockid, int __flags)
{
	auto __fd = real_timerfd_create(__clockid, __flags);
	if (__fd != INVALID)
	{
		auto n = coEpollNode::find(__fd);
		n->set_flags(coEpollEvent::e_actived);
		n->set_flags(coEpollEvent::e_ctrl_async);
	}
	return __fd;
}
