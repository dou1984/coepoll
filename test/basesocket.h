#ifndef __BASE_SOCKET_H__
#define __BASE_SOCKET_H__
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>

#define INVALID (-1)

typedef int SOCKET;

int32_t BindLocal(SOCKET fd, const char *ip);

int32_t Bind(SOCKET fd, const char *ip, int32_t port);

int32_t Listen(SOCKET fd, int count);

int32_t ConnectLocal(SOCKET fd, const char *ip);

int32_t Connect(SOCKET fd, const char *ip, int32_t port);

int GetSocketError(SOCKET fd);

SOCKET Accept(SOCKET fd, char *ip, int len, int *port);

SOCKET AcceptEx(SOCKET fd);

SOCKET AcceptLocal(SOCKET fd, char *ip, int len);

int32_t SetNoBlock(SOCKET fd, int block);

int32_t SetKeepAlive(SOCKET fd, int alive);

int32_t SetTTL(SOCKET fd, int32_t ttl);

int32_t GetTTL(SOCKET fd, int32_t *ttl);

int32_t SetTOS(SOCKET fd, int32_t tos);

int32_t GetTOS(SOCKET fd, int32_t *tos);

int32_t SetReuseAddr(SOCKET fd, int reuse);

int32_t SetSendTimeOut(SOCKET fd, int32_t _timeout);

int32_t SetRecvTimeOut(SOCKET fd, int32_t _timeout);

int32_t SetSendBuffSize(SOCKET fd, int32_t bufsize);

int32_t SetRecvBuffSize(SOCKET fd, int32_t bufsize);

int32_t GetSendBuffSize(SOCKET fd, int32_t *bufsize);

int32_t GetRecvBuffSize(SOCKET fd, int32_t *bufsize);

int32_t SetLingerOff(SOCKET fd);

int32_t SetLingerOn(SOCKET fd, uint16_t _timeout);

int32_t SetNoDelay(SOCKET fd, int nodelay);

int32_t GetSockName(SOCKET fd, char *ip, int len, int32_t *port);

int32_t GetPeerName(SOCKET fd, char *ip, int len, int32_t *port);

int32_t GetHostByName(const char *name, char *ip, int len);

#endif
