#ifndef __COCLIENT_H__
#define __COCLIENT_H__
#include <string>
#include "Common.h"

namespace ashan
{
	class coClient
	{
		int m_fd = INVALID;
		std::string m_ip;
		int m_port = 0;

	public:
		coClient(std::string ip, int port);
		virtual ~coClient();
		int Start();
		int Wait();
		int Send(const char *, int);
	};
}
#endif