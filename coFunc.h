#ifndef __COHOOK_HPP__
#define __COHOOK_HPP__
//#include <bits/sigset.h>
#include <sys/types.h>
#ifdef __cplusplus
extern "C"
{
#endif

	struct epoll_event;
	struct sockaddr;

	int co_socket(int __af, int __type, int __protocol);
	int co_close(int __fd);
	ssize_t co_send(int fd, const char *buf, int len, int flags);
	ssize_t co_recv(int fd, char *buf, int len, int flags);
	ssize_t co_recvfrom(int fd, char *buf, int len, int flags, struct sockaddr *from, int *fromlen);
	ssize_t co_sendto(int fd, const char *buf, int len, int flags, const struct sockaddr *to, int tolen);
	ssize_t co_read(int fd, void *buf, size_t len);
	ssize_t co_write(int fd, const void *buf, size_t len);
	int co_connect(int fd, const struct sockaddr *addr, socklen_t addrlen);
	int co_accept(int fd, struct sockaddr *addr, socklen_t *addrlen);
	int co_fcntl(int __fd, int __cmd, ...);
	unsigned int co_sleep(unsigned int);
	int co_usleep(useconds_t);
	int co_epoll_create(int __size);
	int co_epoll_create1(int __flag);
	int co_epoll_ctl(int __epfd, int __op, int __fd, struct epoll_event *__event);
	int co_epoll_wait(int __epfd, struct epoll_event *__event, int __maxevents, int __timeout);
	int co_epoll_pwait(int __epfd, struct epoll_event *__event, int __maxevents, int __timeout, const __sigset_t *__ss);
	int co_eventfd(unsigned int __initval, int __flags);
	int co_timerfd_create(int __clockid, int __flags);
	

#ifdef __cplusplus
}
#endif

#endif