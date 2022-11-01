#ifndef __COCOMMON_H__
#define __COCOMMON_H__
#include "coContext.h"

namespace ashan
{
	int get_epfd();
	int ctx_start(std::function<void()> &&);
	int ctx_suspend(std::list<std::shared_ptr<coContext>> &__list, bool retry);
	int ctx_suspend_mtx(std::list<std::shared_ptr<coContext>> &_list_ctx, coLock &_mtx);
	int ctx_pushback(std::shared_ptr<coContext> &);
	int ctx_popback();
	int ctx_resume(std::shared_ptr<coContext> &);
}
#endif