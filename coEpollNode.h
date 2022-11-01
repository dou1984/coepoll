#ifndef __COEPOLLNODE_H__
#define __COEPOLLNODE_H__
#include <vector>
#include <list>
#include "Common.h"
#include "coEpollEvent.h"
#include "coLock.h"

namespace ashan
{
	class coContext;
	class coEpollNode final
	{
		std::vector<std::shared_ptr<coEpollEvent>> m_node;
		std::list<std::shared_ptr<coContext>> m_trigger;
		coLock m_mtx;

	public:
		std::shared_ptr<coEpollEvent> get(int __fd);
		void push_back(int __epfd, int __fd, struct epoll_event *__event);
		void clear(int __fd);

		void push_trigger(std::shared_ptr<coContext> &);
		void resume_trigger();

		static coEpollNode *at();
		static std::shared_ptr<coEpollEvent> find(int __fd);
	};
}
#endif