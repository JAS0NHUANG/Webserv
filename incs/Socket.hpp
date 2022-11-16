#ifndef _SOCKET_HPP_
# define _SOCKET_HPP_

# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>

#include "webserv.hpp"

int create_socket(void);

class Socket {
	private:
		short			_port;
		int				_sock_fd;
		sockaddr_in		_addr;
		size_t			_addr_len;
		
	public:
		// canonical
		Socket();
		~Socket();
		Socket(const Socket &toCopy);
		Socket&			operator=(const Socket &toAssign);

		Socket(int port, std::string address);
		// getter
		int				getPort(void);
		int				getSockFd(void);
		int				getAddr(void);
		int				getAddrLen(void);
};

#endif
