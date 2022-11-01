#ifndef __COCONTEXT_H__
#define __COCONTEXT_H__
#include <list>
#include <functional>
#include <memory>
#include <mutex>
#include <boost/context/continuation.hpp>
#include "coLock.h"

namespace ashan
{
	namespace ctx = boost::context;
	class coContext final : public std::enable_shared_from_this<coContext>
	{
		ctx::continuation m_top;
		ctx::continuation m_bottom;
		std::function<void()> m_func;
		int m_thdid = 0;

	public:
		coContext(std::function<void()> &&__f);
		int init();
		int suspend();
		int resume();
		int thdid() { return m_thdid; }

		static std::shared_ptr<coContext> create(std::function<void()> &&__f);
	};

}
#endif