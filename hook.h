#ifndef __COHOOKDL_H__
#define __COHOOKDL_H__

#include "Common.h"

typedef int (*t_socket)(int, int, int);
typedef int (*t_close)(int);
typedef ssize_t (*t_send)(int s, const char *buf, int len, int flags);
typedef ssize_t (*t_recv)(int s, char *buf, int len, int flags);
typedef ssize_t (*t_recvfrom)(int s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen);
typedef ssize_t (*t_sendto)(int s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen);
typedef ssize_t (*t_read)(int fd, void *buf, size_t len);
typedef ssize_t (*t_write)(int fd, const void *buf, size_t len);
typedef int (*t_connect)(int fd, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*t_accept)(int fd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*t_fcntl)(int __fd, int __cmd, ...);
typedef unsigned int (*t_sleep)(unsigned int s);
typedef int (*t_usleep)(useconds_t ms);
typedef int (*t_epoll_create)(int __size);
typedef int (*t_epoll_create1)(int __flags);
typedef int (*t_epoll_ctl)(int __epfd, int __op, int __fd, struct epoll_event *__event);
typedef int (*t_epoll_wait)(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout);
typedef int (*t_epoll_pwait)(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout, const __sigset_t *__ss);
typedef int (*t_eventfd)(unsigned int __initval, int __flags);
typedef int (*t_timerfd_create)(int __clockid, int __flags);
typedef int (*t_pthread_create)(pthread_t *tidp,const pthread_attr_t *attr, void *(*start_rtn)(void*),void *arg);
typedef int (*t_pthread_join)(pthread_t thread, void **retval);
typedef int (*t_pthread_detach)(pthread_t thread);

extern t_socket real_socket;
extern t_close real_close;
extern t_send real_send;
extern t_recv real_recv;
extern t_recvfrom real_recvfrom;
extern t_sendto real_sendto;
extern t_read real_read;
extern t_write real_write;
extern t_connect real_connect;
extern t_accept real_accept;
extern t_fcntl real_fcntl;
extern t_sleep real_sleep;
extern t_usleep real_usleep;
extern t_epoll_create real_epoll_create;
extern t_epoll_create1 real_epoll_create1;
extern t_epoll_ctl real_epoll_ctl;
extern t_epoll_wait real_epoll_wait;
extern t_epoll_pwait real_epoll_pwait;
extern t_eventfd real_eventfd;
extern t_timerfd_create real_timerfd_create;
extern t_pthread_create real_pthread_create;
extern t_pthread_join real_pthread_join;
extern t_pthread_detach real_pthread_detach;

int init_hook();

#endif