#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include "hook.h"

t_socket real_socket = NULL;
t_close real_close = NULL;
t_send real_send = NULL;
t_recv real_recv = NULL;
t_recvfrom real_recvfrom = NULL;
t_sendto real_sendto = NULL;
t_read real_read = NULL;
t_write real_write = NULL;
t_connect real_connect = NULL;
t_accept real_accept = NULL;
t_fcntl real_fcntl = NULL;
t_sleep real_sleep = NULL;
t_usleep real_usleep = NULL;
t_epoll_create real_epoll_create = NULL;
t_epoll_create1 real_epoll_create1 = NULL;
t_epoll_ctl real_epoll_ctl = NULL;
t_epoll_wait real_epoll_wait = NULL;
t_epoll_pwait real_epoll_pwait = NULL;
t_eventfd real_eventfd = NULL;
t_timerfd_create real_timerfd_create = NULL;
t_pthread_create real_pthread_create = NULL;
t_pthread_join real_pthread_join = NULL;
t_pthread_detach real_pthread_detach = NULL;

int init_hook()
{
	static int __init = 0;
	if (__init != 0)
		return __init;
	__init = 1;
	real_socket = (t_socket)dlsym(RTLD_NEXT, "socket");
	real_close = (t_close)dlsym(RTLD_NEXT, "close");
	real_send = (t_send)dlsym(RTLD_NEXT, "send");
	real_recv = (t_recv)dlsym(RTLD_NEXT, "recv");
	real_recvfrom = (t_recvfrom)dlsym(RTLD_NEXT, "recvfrom");
	real_sendto = (t_sendto)dlsym(RTLD_NEXT, "sendto");
	real_read = (t_read)dlsym(RTLD_NEXT, "read");
	real_write = (t_write)dlsym(RTLD_NEXT, "write");
	real_connect = (t_connect)dlsym(RTLD_NEXT, "connect");
	real_accept = (t_accept)dlsym(RTLD_NEXT, "accept");
	real_fcntl = (t_fcntl)dlsym(RTLD_NEXT, "fcntl");
	real_sleep = (t_sleep)dlsym(RTLD_NEXT, "sleep");
	real_usleep = (t_usleep)dlsym(RTLD_NEXT, "usleep");
	real_epoll_create = (t_epoll_create)dlsym(RTLD_NEXT, "epoll_create");
	real_epoll_create1 = (t_epoll_create1)dlsym(RTLD_NEXT, "epoll_create1");
	real_epoll_ctl = (t_epoll_ctl)dlsym(RTLD_NEXT, "epoll_ctl");
	real_epoll_wait = (t_epoll_wait)dlsym(RTLD_NEXT, "epoll_wait");
	real_epoll_pwait = (t_epoll_pwait)dlsym(RTLD_NEXT, "epoll_pwait");
	real_eventfd = (t_eventfd)dlsym(RTLD_NEXT, "eventfd");
	real_timerfd_create = (t_timerfd_create)dlsym(RTLD_NEXT, "timerfd_create");
	real_pthread_create = (t_pthread_create)dlsym(RTLD_NEXT, "pthread_create");
	real_pthread_join = (t_pthread_join)dlsym(RTLD_NEXT, "pthread_join");
	real_pthread_detach = (t_pthread_detach)dlsym(RTLD_NEXT, "pthread_detach");

	return __init;
}
