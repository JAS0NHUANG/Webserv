
#include "../../incs/webserv.hpp"
#include <fcntl.h>
Socket::Socket() {
	this->_port = 4242;
	// this struct is needed to setsockopt and bind
	sockaddr_in	sock_addr;

	// create socket
	if ((this->_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		errMsgErrno("socket failed");
        exit(EXIT_FAILURE);
	}
	// std::cout << "This is the socket fd: " << sock_fd << "\n";

	// setup socket
	ft_memset((char *) &sock_addr, 0, (sizeof(sock_addr)));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_addr.sin_port = htons(this->_port);

	// this will make addr reusable?!
	if (setsockopt(this->_sock_fd, SOL_SOCKET, SO_REUSEADDR, &sock_addr, \
		sizeof(sock_addr)) < 0) {
		errMsgErrno("setsockopt failed");
        exit(EXIT_FAILURE);
	}

	// bind
	if (bind(this->_sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0) {
		errMsgErrno("bind failed");
		exit(EXIT_FAILURE);
	}
	// std::cout << "Bind to port " << port << " successful.\n";

	// listen
	if (listen(this->_sock_fd, 1024) < 0) {
		errMsgErrno("listen failed");
    	exit(EXIT_FAILURE);
	}
	// std::cout << "Listening on " << sock_fd << " \n";

	// try to set the socket non-blocking
	std::cout << "FD flag: " << fcntl(this->_sock_fd, F_GETFD) << "\n";
	std::cout << "fd " << this->_sock_fd;
}

Socket::~Socket() {
	std::cout << "socket distructor!!\n";
}

/*
Socket::Socket(const Socket &toCopy) {
}
*/

int	create_socket(void) {
	/* accept, recv and send
	while(1){
		// accept connection
		int accept_sock_fd = accept(sock_fd, (struct sockaddr*)&sock_addr, (unsigned int*)&sock_addr.sin_len);

		// recv from the connection
		char buffer[2048] = {0};
		recv(accept_sock_fd, buffer, 2048, 0);
		std::cout << "The request message was: \n\n" << buffer;

		// send a message/response to the connection
		std::string response =
			"HTTP/1.1 9876543210 crazy status code\n"
			"Date: Thu, 19 Feb 2009 12:27:04 GMT\n"
			"Server: Apache/2.2.3\n"
			"Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
			"ETag: \"56d-9989200-1132c580\"\n"
			"Content-Type: text/html\n"
			"Content-Length: 15\n"
			"Accept-Ranges: bytes\n"
			"42Header: Jason/WenTsu/Stone\n"
			"\n"
			"<h1>HELLO!!!</h1>";
		 send(accept_sock_fd, response.c_str(), response.size(), 0);
	}
	*/
	return (0);
};
