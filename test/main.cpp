#include <sys/epoll.h>
#include <assert.h>
#include <coMutex.h>
#include <coEpoll.h>
#include <Common.h>
#include "basesocket.h"
#include "coServer.h"
#include "coClient.h"
#include "thdid.h"

using namespace ashan;

static const uint16_t port = 12000;

void shedule()
{
	const int stimer = 1000000;
	while (true)
	{
		usleep(stimer);
		CO_INF("sleep %d", stimer);
	}
}

int test_mutex()
{
	static coMutex _mtx;
	CO_INF("lock thdid:%d", g_thdid);
	_mtx.lock();
	CO_INF("sleep begin %d", g_thdid);
	sleep(2);
	CO_INF("sleep end %d", g_thdid);
	_mtx.unlock();
	CO_INF("unlock thdid:%d", g_thdid);
	return 0;
}

int test_fast_mutex()
{
	static coMutex _mtx;
	while (true)
	{
		_mtx.lock();
		_mtx.unlock();
	}
	return 0;
}
int test_spinlock()
{
	while (true)
	{
		coLock _mtx;
		_mtx.lock();
		_mtx.unlock();
	}
	return 0;
}
int main_tx()
{
	co[]()
	{
		auto epfd = epoll_create(0);

		while (true)
		{
			const int _count = 100;
			struct epoll_event _e[_count];
			int r = epoll_wait(epfd, _e, _count, -1);
		}
	};
	co_main([]()
			{ sleep(10000000); });
	return 0;
}
int main_test(int argc, char **argv)
{

	auto f = []()
	{
		CO_INF("client begin %d", g_thdid);
		sleep(1);
		CO_INF("client sleep %d", g_thdid);
		coClient cli("0.0.0.0", 12000);
		cli.Start();
		char buf[] = "01234567890123456789";
		int len = strlen(buf);
		cli.Send(buf, len);
		cli.Wait();
		CO_INF("client stop %d", g_thdid);
	};
	int thd_max = 20;
	for (int i = 0; i < thd_max; i++)
	{
		co_rand f;
	}

	co_main(
		[]()
		{
			coServer srv("0.0.0.0", 12000);
			srv.Start();
		});
	return 0;
}
int main(int argc, char **argv)
{
	return main_test(argc, argv);
}