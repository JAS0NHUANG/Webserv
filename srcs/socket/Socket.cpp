#include "Socket.hpp"

Socket::Socket(void) {};

Socket::~Socket(void) {};

Socket::Socket(const Socket &toCopy) {};

Socket	&Socket::operator=(const Socket &toAssign) {
	return (*this);
};

int	Socket::run(void) {
	int	socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	std::cout << "This is the socket fd: " << socket_fd << "\n";
	if (socket_fd == -1) {
		std::cout << "Failed to create socket.\n";
	}

};

