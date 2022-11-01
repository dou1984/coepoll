#ifndef __COSERVER_H__
#define __COSERVER_H__
#include <string>
#include <vector>
#include <Common.h>

namespace ashan
{
	class coServer
	{
		std::string m_ip = "127.0.0.1";
		int m_port = 0;
		int m_accept_fd = INVALID;

	public:
		coServer(std::string ip, int port);
		virtual ~coServer();
		void Start();
	};
}
#endif