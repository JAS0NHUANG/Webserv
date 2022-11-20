#include "../../incs/webserv.hpp"

Socket::Socket(int port, std::string address, Server conf) :
	_port(port),
	_conf(conf) {

	// create socket
	if ((this->_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throwError("socket");

	// this struct is needed to setsockopt and bind
	sockaddr_in	sock_addr;
	ft_memset((char *) &sock_addr, 0, (sizeof(sock_addr)));

	// setup socket
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(address.c_str());
	sock_addr.sin_port = htons(this->_port);

	std::cout << "\n";
	std::cout << BLU << "socket object:\naddress str: " << address << ", s_addr: " << sock_addr.sin_addr.s_addr << "\n";

	// this will make addr reusable?!
	if (setsockopt(this->_sock_fd, SOL_SOCKET, SO_REUSEADDR, &sock_addr, \
		sizeof(sock_addr)) < 0)
		throwError("setsockopt");

	// bind
	if (bind(this->_sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
		throwError("bind");
	std::cout << "binding ok on sock addr: " << sock_addr.sin_addr.s_addr << "\n";

	// listen
	if (listen(this->_sock_fd, 1024) < 0)
		throwError("listen");
	std::cout << "Listening on socket fd " << _sock_fd << " \n";
}

Socket::Socket() {
	this->_port = 4242;
	// The original socket that was set up for listening is used only for accepting connections, not for exchanging data.

	// create socket
	if ((this->_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throwError("socket");

	// this struct is needed to setsockopt and bind
	sockaddr_in	sock_addr;
	ft_memset((char *) &sock_addr, 0, (sizeof(sock_addr)));

	// setup socket
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_addr.sin_port = htons(this->_port);

	// this will make addr reusable?!
	if (setsockopt(this->_sock_fd, SOL_SOCKET, SO_REUSEADDR, &sock_addr, \
		sizeof(sock_addr)) < 0)
		throwError("setsockopt");

	// bind
	if (bind(this->_sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
		throwError("bind");

	// listen
	if (listen(this->_sock_fd, 1024) < 0)
		throwError("listen");
	std::cout << "Listening on socket fd " << _sock_fd << " \n";
}

Socket::~Socket() {}

Socket::Socket(const Socket &toCopy) {
	*this = toCopy;
}

Socket&	Socket::Socket::operator=(const Socket &toAssign) {
	this->_port		= toAssign._port;
	this->_sock_fd	= toAssign._sock_fd;
	this->_addr		= toAssign._addr;
	this->_addr_len	= toAssign._addr_len;
	this->_conf		= toAssign._conf;
	return (*this);
}

int	Socket::getPort(void) {
	return (this->_port);
}

int	Socket::getSockFd(void) {
	return (this->_sock_fd);
}

Server Socket::getConf(void) {
	return _conf;
}