#include <unistd.h>
#include <coEpoll.h>
#include <co.h>
#include "basesocket.h"
#include "coServer.h"
#include "ioBuffer.h"

namespace ashan
{
	void coServerRecv(int sockfd)
	{
		ioBufferEx<char> iobuf;
		iobuf.set_expand_size(0x1000);
		do
		{
			auto n = iobuf.append(
				[=](auto buf, auto size)
				{ return read(sockfd, buf, size); });
			if (n > 0)
			{
				CO_INF("read fd:%d %ld %s", sockfd, n, iobuf.data());
				auto r = write(sockfd, iobuf.data(), n);
				iobuf.pop_front(r);
			}
			else
			{
				CO_INF("read fd:%d %ld error:%s", sockfd, n, strerror(errno));
				close(sockfd);
				break;
			}
		} while (true);
	}
	void coServerAcceptor(int accept_fd)
	{
		co_setactive(accept_fd, 1);
		do
		{
			CO_INF("fd:%d begin AcceptEx", accept_fd);
			int sockfd = AcceptEx(accept_fd);
			if (sockfd == INVALID)
			{
				CO_INF("accept error:%s", strerror(errno));
				break;
			}
			CO_INF("fd:%d accept fd:%d", accept_fd, sockfd);
			co[=]()
			{
				coServerRecv(sockfd);
			};
		} while (true);

		co_setactive(accept_fd, 0);
	}
	coServer::coServer(std::string ip, int port) : m_ip(ip), m_port(port)
	{
		m_accept_fd = socket(AF_INET, SOCK_STREAM, 0);
		SetReuseAddr(m_accept_fd, 1);
	}
	coServer::~coServer()
	{
		if (m_accept_fd != INVALID)
		{
			close(m_accept_fd);
		}
	}
	void coServer::Start()
	{
		if (Bind(m_accept_fd, m_ip.c_str(), m_port) == INVALID)
		{
			CO_INF("bind error please change the port value %s", strerror(errno));
			return;
		}
		if (Listen(m_accept_fd, 5) == INVALID)
		{
			CO_INF("listen error %s", strerror(errno));
			return;
		}

		CO_DBG("coServerAcceptor begin %d", m_accept_fd);
		coChannel<int> ch;
		co_all[&]()
		{
			coServerAcceptor(m_accept_fd);
		};
		int r;
		ch >> r;
		CO_INF("echo_server error thread quit %d", 0);
	}
}