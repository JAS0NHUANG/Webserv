#include "webserv.hpp"

Socket::Socket(int port, std::string address, std::vector<Config> _virtual_servers) :
	_port(port),
	_virtual_servers(_virtual_servers) {

	(void)address;
	struct addrinfo hints;
    struct addrinfo *result, *rp;
	int s;
	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
	std::string port_string = to_String(port);

	if ((s = getaddrinfo(NULL, port_string.c_str(), &hints, &result)) != 0){
		gai_strerror(s);
		throwError("getaddrinfo");
	}
	 /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully bind(2).
       If socket(2) (or bind(2)) fails, we (close the socket
       and) try the next address. */
	
	for (rp = result; rp != NULL; rp = rp->ai_next) {
        this->_sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (this->_sock_fd  == -1)
            continue;
		int yes = 1;
		if (setsockopt(this->_sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
			throwError("setsockopt");
       	if (bind(this->_sock_fd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */
       	close(this->_sock_fd);
    }
	if (rp == NULL) {               /* No address succeeded */
        throwError("bind");
    }
	//freeaddrinfo(result);
	// listen
	if (listen(this->_sock_fd, 1024) < 0)
		throwError("listen");
}

Socket::Socket() {
	this->_port = 4242;
	
	struct addrinfo hints;
    struct addrinfo *result, *rp;
	int s;
	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
	std::string port_string = to_String(this->_port);

	if ((s = getaddrinfo(NULL, port_string.c_str(), &hints, &result)) != 0){
		gai_strerror(s);
		throwError("getaddrinfo");
	}
	 /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully bind(2).
       If socket(2) (or bind(2)) fails, we (close the socket
       and) try the next address. */
	
	for (rp = result; rp != NULL; rp = rp->ai_next) {
        this->_sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (this->_sock_fd == -1)
            continue;
		int yes = 1;
		if (setsockopt(this->_sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
			throwError("setsockopt");
       	if (bind(this->_sock_fd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */
       	close(this->_sock_fd);
    }
	if (rp == NULL) {               /* No address succeeded */
        throwError("bind");
    }
	//freeaddrinfo(result);

	// listen
	if (listen(this->_sock_fd, 1024) < 0)
		throwError("listen");
}

Socket::~Socket() {}

Socket::Socket(const Socket &toCopy) {
	*this = toCopy;
}

Socket&	Socket::Socket::operator=(const Socket &toAssign) {
	this->_port				= toAssign._port;
	this->_sock_fd			= toAssign._sock_fd;
	this->_addr				= toAssign._addr;
	this->_addr_len			= toAssign._addr_len;
	this->_virtual_servers	= toAssign._virtual_servers;
	return (*this);
}

int	Socket::getPort(void) {
	return (this->_port);
}

int	Socket::getSockFd(void) {
	return (this->_sock_fd);
}

std::vector<Config> Socket::get_virtual_servers(void) {
	return _virtual_servers;
}
