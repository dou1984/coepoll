#include <unistd.h>
#include "basesocket.h"
#include "coClient.h"
#include "ioBuffer.h"

namespace ashan
{
	coClient::coClient(std::string ip, int port) : m_ip(ip), m_port(port)
	{
		m_fd = socket(AF_INET, SOCK_STREAM, 0);
	}
	coClient::~coClient()
	{
		close(m_fd);
	}
	int coClient::Start()
	{
		if (Connect(m_fd, m_ip.c_str(), m_port) == INVALID)
		{
			CO_INF("connect %d error:%s", m_fd, strerror(errno));
			return INVALID;
		}
		CO_INF("connect success fd:%d %s:%d", m_fd, m_ip.c_str(), m_port);
		return 0;
	}
	int coClient::Wait()
	{
		ioBufferEx<char> iobuf;
		iobuf.set_expand_size(0x1000);
		do
		{
			auto n = iobuf.append(
				[this](auto _buf, auto _size)
				{ return recv(m_fd, _buf, _size, 0); });
			if (n > 0)
			{
				CO_INF("fd:%d recv %ld:%s", m_fd, n, iobuf.data());
				
				Send(iobuf.data(), iobuf.size());
				iobuf.pop_front(n);
			}
			else
			{
				assert(n != 0);
				CO_INF("error fd:%d recv %ld:%s", m_fd, n, iobuf.data());
				break;
			}
		} while (true);
		return 0;
	}
	int coClient::Send(const char *_buf, int _size)
	{
		CO_DBG("send fd:%d begin", m_fd);
		return send(m_fd, _buf, _size, 0);
	}
}