#ifndef _SOCKET_HPP_
# define _SOCKET_HPP_

# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>

# include "webserv.hpp"

class Socket {
	private:
		// put into a struct socket
		short			_port;
		int				_sock_fd;
		sockaddr_in		_addr;
		size_t			_addr_len;
		Config			_conf;

		// save configuration for this address:port

	public:
		// canonical
		Socket();
		~Socket();
		Socket(const Socket &toCopy);
		Socket&			operator=(const Socket &toAssign);

		Socket(int port, std::string address, Config conf);
		// getter
		int				getPort(void);
		int				getSockFd(void);
		int				getAddr(void);
		int				getAddrLen(void);
		Config			getConf(void);
};

#endif
