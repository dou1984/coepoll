#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include "Common.h"
#include "coTrigger.h"
#include "coContext.h"
#include "coEpollNode.h"
#include "coEpollEvent.h"
#include "coFunc.h"
#include "coLock.h"
#include "thdid.h"

namespace ashan
{
	static std::vector<coTrigger *> g_trigger;
	static coLock g_mtx;

	coTrigger::coTrigger()
	{
		m_fd = real_eventfd(0, EFD_NONBLOCK);
		if (m_fd < 0)
		{
			CO_DBG("create real_eventfd error:%d %s", errno, strerror(errno));
		}
		m_node = coEpollNode::at();
		auto n = m_node->get(m_fd);
		n->set_noblock(m_fd);
		n->set_flags(coEpollEvent::e_actived);
		n->set_flags(coEpollEvent::e_threadsafe);
		n->set_flags(coEpollEvent::e_autoread);
		n->set_epoll_event(m_fd, EPOLLIN | EPOLLET);
		CO_DBG("coTrigger thdid:%d real_eventfd %d", g_thdid, m_fd);

		m_thdid = g_thdid;
		std::lock_guard<coLock> _lock(g_mtx);
		if (m_thdid >= g_trigger.size())
		{
			g_trigger.resize(m_thdid + 1);
		}
		g_trigger[m_thdid] = this;
	}
	coTrigger::~coTrigger()
	{
		auto n = coEpollNode::find(m_fd);
		n->del_epoll_event(m_fd);
		real_close(m_fd);
		std::lock_guard<coLock> _lock(g_mtx);
		g_trigger[m_thdid] = nullptr;
	}
	int coTrigger::on_trigger(std::shared_ptr<coContext> &_ctx)
	{
		CO_DBG("on_trigger thdid:%d", g_thdid);
		m_node->push_trigger(_ctx);
		return __trigger();
	}
	int coTrigger::__trigger()
	{
		CO_DBG("trigger begin thdid:%d %d fd:%d", m_thdid, g_thdid, m_fd);
		unsigned long long reserve = 1;
		int r = real_write(m_fd, &reserve, sizeof(reserve));
		if (r < 0)
		{
			CO_DBG("on_event error %s", strerror(errno));
		}
		return r;
	}
	int coTrigger::on(std::shared_ptr<coContext> &_ctx)
	{
		int thdid = _ctx->thdid();
		CO_DBG("coTrigger::on %d this:%d", thdid, g_thdid);
		if (thdid == g_thdid)
		{
			return _ctx->resume();
		}
		auto _trigger = coTrigger::at(thdid);
		if (_trigger != nullptr)
		{
			return _trigger->on_trigger(_ctx);
		}
		return 0;
	}
	coTrigger *coTrigger::at(int __fd)
	{
		assert(__fd >= 0);
		if (__fd < thdid::instance().max())
		{
			std::lock_guard<coLock> _lock(g_mtx);
			assert(g_trigger[__fd]);
			return g_trigger[__fd];
		}
		return nullptr;
	}
	int coTrigger::max()
	{
		return thdid::instance().max();
	}

}