#ifndef __CONNPOOL_H__
#define __CONNPOOL_H__
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <cassert>
#include <mutex>
#include "coLock
#include "ucode.h"

namespace ashan
{
#define DEF_POOL_SIZE (4)
#define ERROR_TAG (-1)

	class coContext;
	extern int ctx_suspend(std::list<std::shared_ptr<coContext>> &__list, bool __retry);
	extern int ctx_resume(std::shared_ptr<coContext> &);

	template <class _Cli>
	class __ConnObj;
	template <class _Cli>
	class __ConnPool;
	template <class _Cli>
	class __ConnPoolSet;

	template <class _Cli>
	class ConnPool final
	{
		const std::string m_url;
		const int m_max_count;
		int m_tag = ERROR_TAG;

	public:
		ConnPool(const std::string &_url, int _count = DEF_POOL_SIZE) : m_url(_url), m_max_count(_count)
		{
		}
		~ConnPool()
		{
			__ConnPoolSet<_Cli>::reset(m_tag);
		}
		__ConnObj<_Cli> get()
		{
			if (m_tag == ERROR_TAG)
			{
				auto &_pool = __ConnPoolSet<_Cli>::at(m_url, m_max_count);
				m_tag = _pool.tag();
				return __ConnObj<_Cli>(_pool.pop(), m_tag);
			}
			auto &_pool = __ConnPoolSet<_Cli>::at(m_tag);
			return __ConnObj<_Cli>(_pool.pop(), m_tag);
		}		
	};

	template <class _Cli>
	class __ConnObj final
	{
		std::shared_ptr<_Cli> m_cli;
		int m_tag = ERROR_TAG;

	public:
		__ConnObj(std::shared_ptr<_Cli> cli, int _tag) : m_cli(cli), m_tag(_tag)
		{
		}
		__ConnObj(__ConnObj &&_obj)
		{
			m_cli = _obj.m_cli;
			m_tag = _obj.m_tag;
		}
		~__ConnObj()
		{
			auto &_pool = __ConnPoolSet<_Cli>::at(m_tag);
			_pool.push(m_cli);
		}
		std::shared_ptr<_Cli> &operator->()
		{
			return m_cli;
		}
		bool valid() const
		{
			return m_cli && m_cli->valid();
		}
		void reset()
		{
			m_cli.reset();	
		}
	};

	template <class _Cli>
	class __ConnPool final
	{
		std::list<std::shared_ptr<_Cli>> m_list;
		std::list<std::shared_ptr<coContext>> m_ctx;
		std::string m_url;
		int m_count = 0;
		int m_max_count = 1;
		int m_tag = ERROR_TAG;

	public:
		__ConnPool() = default;
		~__ConnPool() = default;
		__ConnPool(__ConnPool &&_p)
		{
			*this = std::move(_p);
		}
		void operator=(__ConnPool &&_p)
		{
			m_list = std::move(_p.m_list);
			m_url = std::move(_p.m_url);
			m_count = _p.m_count;
			m_max_count = _p.m_max_count;
			m_tag = _p.m_tag;
		}
		void init(const std::string &_url, int _count, int _tag)
		{
			m_max_count = std::max(m_max_count, _count);
			if (full())
			{
				return;
			}
			auto cli = std::make_shared<_Cli>();
			if (cli->Connect(_url) == 0)
			{
				m_count++;
				m_list.push_back(cli);
				m_url = _url;
				m_tag = _tag;
			}
		}
		bool full() const
		{
			assert(m_count <= m_max_count);
			return m_count == m_max_count;
		}
		bool empty() const { return m_count == 0; }
		int tag() const { return m_tag; }
		void push(std::shared_ptr<_Cli> cli)
		{
			if (!(cli && cli->valid()))
			{
				m_count--;
				init(m_url, m_max_count, m_tag);
			}
			m_list.push_back(cli);
			if (m_ctx.size() > 0)
			{
				auto _ctx = m_ctx.front();
				m_ctx.pop_front();
				ctx_resume(_ctx);
			}
		}
		std::shared_ptr<_Cli> pop()
		{
			if (m_list.empty())
			{
				ctx_suspend(m_ctx, false);
			}
			auto cli = m_list.front();
			m_list.pop_front();
			return cli;
		}
	};
	template <class _Cli>
	class __ConnPoolSet
	{
		static thread_local std::vector<__ConnPool<_Cli>> m_pool;
		static ucode m_code;
		static coLock m_mtx;

	public:
		static __ConnPool<_Cli> &at(const std::string &_url, int _count)
		{
			int _index = 0;
			{
				std::lock_guard<coLock> _lock(m_mtx);
				_index = m_code.getfirst();
			}
			__ConnPool<_Cli> _pool;
			_pool.init(_url, _count, _index);
			if (_index >= m_pool.size())
			{
				m_pool.resize(_index + 1);
			}
			m_pool[_index] = std::move(_pool);
			return m_pool[_index];
		}
		static __ConnPool<_Cli> &at(size_t _idx)
		{
			if (_idx < m_pool.size())
			{
				return m_pool[_idx];
			}
			throw("at error index");
		}
		static void reset(int _index)
		{
			if (_index == ERROR_TAG)
			{
				return;
			}
			std::lock_guard<coLock> _lock(m_mtx);
			m_code.reset(_index);
		}
	};
	template <class _Cli>
	thread_local std::vector<__ConnPool<_Cli>> __ConnPoolSet<_Cli>::m_pool;
	template <class _Cli>
	ucode __ConnPoolSet<_Cli>::m_code;
	template <class _Cli>
	coLock __ConnPoolSet<_Cli>::m_mtx;
}

#endif
