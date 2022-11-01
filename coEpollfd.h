
#ifndef __COEPOLLFD_H__
#define __COEPOLLFD_H__
#include <memory>
#include <bitset>
#include "Common.h"

namespace ashan
{
	class coEpollfd final : std::enable_shared_from_this<coEpollfd>
	{
		int m_fd = INVALID;
		struct epoll_event *m_event = nullptr;
		int m_maxevents = 0;
		int m_size = 0;

	public:
		coEpollfd();
		virtual ~coEpollfd();
		void set(struct epoll_event *__event, int __maxevents, int __size);
		void clear();
		int push_back(int _events, void *_ptr);
		int get_fd() { return m_fd; }
		int size() const { return m_size; }
		int nofull() const { return m_size < m_maxevents; }
		int set_timer(int _stamp);
		int stop_timer();

		static std::shared_ptr<coEpollfd> at(int __fd);
		static std::shared_ptr<coEpollfd> create();
		static void clear(int __fd);
	};
}

#endif
