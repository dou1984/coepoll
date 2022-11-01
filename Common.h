#ifndef __COMMON_H__
#define __COMMON_H__

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <time.h>
#include "comdef.h"

#define TIMEOUT (15.0f)
#define MAX_EPOLL_NODE (0x1000)
#define INVALID (-1)

#define CO_TRY() \
	try          \
	{
#define CO_CATCH()                         \
	}                                      \
	catch (...)                            \
	{                                      \
		CO_DBG("catch thdid:%d", g_thdid); \
	}

#endif
