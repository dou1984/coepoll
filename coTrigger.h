#ifndef __COTRIGGER_H__
#define __COTRIGGER_H__
#include <memory>
#include <list>
#include <mutex>

namespace ashan
{
	class coContext;
	class coEpollNode;
	class coTrigger final
	{
		int m_fd = 0;
		int m_thdid = 0;
		coEpollNode *m_node;

		int on_trigger(std::shared_ptr<coContext> &_ctx);
		int __trigger();

	public:
		coTrigger();
		virtual ~coTrigger();

		static int on(std::shared_ptr<coContext> &_ctx);
		static coTrigger *at(int);
		static int max();
	};
}
#endif
