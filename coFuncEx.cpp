#include <array>
#include <memory>
#include <string.h>
#include "coFuncEx.h"
#include "coThread.h"
#include "coChannel.h"
#include "coEpoll.h"
#include "coContext.h"
#include "coCommon.h"
extern "C"
{
#include "hook.h"
}

static std::array<ashan::coThread, 32> g_thread;
static std::atomic_bool g_thread_building = {true};
static int g_init_cothread = []()
{
	for (int i = 0; i < g_thread.size(); i++)
	{
		g_thread[i].detach();
	}
	for (int i = 0; i < g_thread.size(); i++)
	{
		while (!g_thread[i].isrunning())
		{
			real_usleep(10);
		}
	}
	g_thread_building = false;
	CO_DBG("thread building %ld is success", g_thread.size());
	return 0;
}();
static char corount_name[] = "COROUNT";
struct corount_t
{
	char name[8];
	ashan::coChannel<size_t> channel;
};
int co_pthread_create(pthread_t *tidp, const pthread_attr_t *attr, void *(*start_rtn)(void *), void *arg)
{
	if (g_thread_building)
	{
		return real_pthread_create(tidp, attr, start_rtn, arg);
	}
	auto c = std::make_shared<corount_t>();
	strncpy(c->name, corount_name, sizeof(c->name));
	co_rand[=]()
	{
		(*start_rtn)(arg);
		size_t t = 0;
		c->channel << t;
	};
	tidp = (pthread_t *)c.get();
	return 0;
}
int co_pthread_join(pthread_t thread, void **retval)
{
	if (g_thread_building)
	{
		return real_pthread_join(thread, retval);
	}
	auto c = (corount_t *)(thread);
	if (strcmp(c->name, corount_name) != 0)
	{
		return real_pthread_join(thread, retval);
	}
	size_t t = 0;
	c->channel >> t;
	return 0;
}
int co_pthread_detach(pthread_t thread)
{
	if (g_thread_building)
	{
		return real_pthread_detach(thread);
	}
	return 0;
}
