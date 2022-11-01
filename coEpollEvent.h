#ifndef __CO_EPOLLEVENT_H__
#define __CO_EPOLLEVENT_H__
#include "Common.h"
#include <list>
#include <bitset>
#include <boost/context/continuation.hpp>
#include "coLock.h"
extern "C"
{
#include "hook.h"
}
namespace ashan
{
	namespace ctx = boost::context;
	class coContext;
	struct coEpollEvent final : std::enable_shared_from_this<coEpollEvent>
	{
		enum E_SUSPEND
		{
			e_suspend_read,
			e_suspend_write,
			e_suspend_rw,
		};
		enum E_STATE
		{
			e_actived,	   //用户调用socket
			e_autodisable,
			e_ctrl_async,  //用户主动异步
			e_epollfd,	   //是create_epoll fd
			e_async,	   //同步变异步
			e_settled,	   //设置epoll_event
			e_epoll_rdwr,  //同时设置EPOLLIN EPOLLOUT, accept/connect使用
			e_reenterable, //可重入
			e_threadsafe,  //使用线程安全
			e_autoread,	   //自动读 coTrigger 时使用
			e_ioctl_read,  //是否读完
			e_ioctl_write, //是否写完		
			e_end,
		};
		struct epoll_event _src;
		struct epoll_event _dst;
		int events = 0;
		int epfd = INVALID;

		std::bitset<e_end> flags = {};
		std::list<std::shared_ptr<coContext>> incoro;
		std::list<std::shared_ptr<coContext>> outcoro;
		coLock mtx;

		void set(int __epfd, int fd, struct epoll_event *ev);
		void set(int __epfd, struct epoll_event *ev);
		void reset();
		void set_noblock(int fd);

		void set_flags(int f, int b = 1) { flags.set(f, b); }
		bool is_flags(int f) { return flags[f]; }
		void set_epoll_event(int fd, int __events);
		void mod_epoll_event(int fd, int __events);
		void del_epoll_event(int fd);
		int ctl_epoll_event(int __epfd, int __op, int __fd, struct epoll_event *__event);

		int resume(int flags);
		int suspend(int flags, bool retry = false);

		int __autoread();
		int __resume_trigger();

		bool isactived() const { return flags[e_actived]; }
		bool isctrl() const { return flags[e_ctrl_async]; }
	};
}
#endif