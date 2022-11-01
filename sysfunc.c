#include <assert.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include "comdef.h"
#include "coFunc.h"
#include "coFuncEx.h"

int socket(int __af, int __type, int __protocol)
{
	CO_DBG("socket af:%d", __af);
	return co_socket(__af, __type, __protocol);
}
int close(int __fd)
{
	CO_DBG("close fd:%d", __fd);
	return co_close(__fd);
}
ssize_t send(int fd, const char *buf, int len, int flags)
{
	CO_DBG("send fd:%d size:%d", fd, len);
	return co_send(fd, buf, len, flags);
}
ssize_t recv(int fd, char *buf, int len, int flags)
{
	CO_DBG("recv fd:%d size:%d", fd, len);
	return co_recv(fd, buf, len, flags);
}
ssize_t recvfrom(int fd, char *buf, int len, int flags, struct sockaddr *from, int *fromlen)
{
	return co_recvfrom(fd, buf, len, flags, from, fromlen);
}
ssize_t sendto(int fd, const char *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
	return co_sendto(fd, buf, len, flags, to, tolen);
}
ssize_t read(int fd, void *buf, size_t len)
{
	CO_DBG("read fd:%d size:%ld", fd, len);
	return co_read(fd, buf, len);
}
ssize_t write(int fd, const void *buf, size_t len)
{
	CO_DBG("write fd:%d size:%ld", fd, len);
	return co_write(fd, buf, len);
}
int connect(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	CO_DBG("connect fd:%d", fd);
	return co_connect(fd, addr, addrlen);
}
int accept(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
	CO_DBG("accept fd:%d", fd);
	return co_accept(fd, addr, addrlen);
}
int fcntl(int __fd, int __cmd, ...)
{
	EXPAND_PARAM(__cmd)
	return co_fcntl(__fd, __cmd, __arg);
}
unsigned int sleep(unsigned int t)
{
	return co_sleep(t);
}
int usleep(useconds_t t)
{
	return co_usleep(t);
}
int epoll_create(int __size)
{
	return co_epoll_create(__size);
}
int epoll_create1(int __flag)
{
	return co_epoll_create1(__flag);
}
int epoll_ctl(int __epfd, int __op, int __fd, struct epoll_event *__event)
{
	return co_epoll_ctl(__epfd, __op, __fd, __event);
}
int epoll_wait(int __epfd, struct epoll_event *__event, int __maxevents, int __timeout)
{
	return co_epoll_wait(__epfd, __event, __maxevents, __timeout);
}
int epoll_pwait(int __epfd, struct epoll_event *__event, int __maxevents, int __timeout, const __sigset_t *__ss)
{
	return co_epoll_pwait(__epfd, __event, __maxevents, __timeout, __ss);
}
int eventfd(unsigned int __initval, int __flags)
{
	return co_eventfd(__initval, __flags);
}
int timerfd_create(int __clockid, int __flags)
{
	return co_timerfd_create(__clockid, __flags);
}
int pthread_create(pthread_t *tidp, const pthread_attr_t *attr, void *(*start_rtn)(void *), void *arg)
{
	return co_pthread_create(tidp, attr, start_rtn, arg);
}
int pthread_join(pthread_t thread, void **retval)
{
	return co_pthread_join(thread, retval);
}
int pthread_detach(pthread_t thread)
{
	return co_pthread_detach(thread);
}