#include "Socket.hpp"

Socket::Socket(void) {};

Socket::~Socket(void) {};

Socket::Socket(const Socket &toCopy) {};

Socket	&Socket::operator=(const Socket &toAssign) {
	return (*this);
};

int	Socket::run(void) {
	int	port;
	sockaddr_in	socket_addr;

	int	socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	std::cout << "This is the socket fd: " << socket_fd << "\n";
	if (socket_fd == -1) {
		std::cout << "Failed to create socket.\n";
	}

	port = 4242;
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = INADDR_ANY;
	socket_addr.sin_port = htons(port);
	if (bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) < 0) {
		std::cout << "Failed to bind to port " << port << ". error: " << strerror(errno) << "\n";
	} else {
		std::cout << "Bind to port " << port << " successful.\n";
	}

	return (0);
};
