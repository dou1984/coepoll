#include <errno.h>
#include "Common.h"
#include "coContext.h"
#include "coCommon.h"
#include "coFunc.h"
#include "coEpollNode.h"
#include "thdid.h"

namespace ashan
{
	coContext::coContext(std::function<void()> &&__f)
	{
		m_func = std::move(__f);
		m_thdid = g_thdid;
	}
	int coContext::init()
	{
		m_top = ctx::callcc(
			[this](auto &&sink)
			{
				m_bottom = std::move(sink);
				auto _ctx = shared_from_this();
				ctx_pushback(_ctx);
				CO_TRY()
				m_func();
				CO_CATCH()
				ctx_popback();
				return std::move(m_bottom);
			});
		CO_DBG("coContext init thdid:%d", g_thdid);
		return 0;
	}
	int coContext::suspend()
	{
		assert(m_bottom);
		m_bottom = std::move(m_bottom).resume();
		return 0;
	}
	int coContext::resume()
	{
		assert(m_top);
		m_top = std::move(m_top).resume();
		return 0;
	}
	std::shared_ptr<coContext> coContext::create(std::function<void()> &&__f)
	{
		auto c = std::make_shared<coContext>(std::move(__f));
		c->init();
		return c;
	}
}