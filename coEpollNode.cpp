#include <string.h>
#include <mutex>
#include "thdid.h"
#include "coEpollNode.h"
#include "coContext.h"

#include <chrono>

namespace ashan
{
	using namespace std::chrono;
	void coEpollNode::push_back(int __epfd, int __fd, struct epoll_event *__event)
	{
		auto n = get(__fd);
		n->set(__epfd, __fd, __event);
	}
	void coEpollNode::clear(int __fd)
	{
		if (__fd < m_node.size())
		{
			auto n = m_node[__fd];
			n->reset();
		}
	}
	void coEpollNode::push_trigger(std::shared_ptr<coContext> &_ctx)
	{
		std::lock_guard<coLock> _lock(m_mtx);
		m_trigger.push_back(_ctx);
	}
	void coEpollNode::resume_trigger()
	{
		decltype(m_trigger) _trigger;
		{
			std::lock_guard<coLock> _lock(m_mtx);
			_trigger = std::move(m_trigger);
		}
		CO_DBG("resume_trigger %ld", _trigger.size());
		if (!_trigger.empty())
		{
			_trigger.front()->resume();
			_trigger.pop_front();
		}
	}
	coEpollNode *coEpollNode::at()
	{
		static thread_local struct coEpollNode _epoll_node;
		return &_epoll_node;
	}
	std::shared_ptr<coEpollEvent> coEpollNode::find(int __fd)
	{
		return coEpollNode::at()->get(__fd);
	}
	std::shared_ptr<coEpollEvent> coEpollNode::get(int __fd)
	{
		assert(__fd >= 0);
		if (__fd >= m_node.size())
		{
			m_node.resize(__fd + 1);
		}
		auto e = m_node[__fd];
		if (e == nullptr)
		{
			m_node[__fd] = e = std::make_shared<coEpollEvent>();
		}
		return e;
	}
}