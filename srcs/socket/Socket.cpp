#include "../../incs/Socket.hpp"
#include <fcntl.h>

int	create_socket(void) {
	int	port;
	// this struct is needed to setsockopt and bind
	sockaddr_in	socket_addr;
	int socket_fd;
	int	accept_sock_fd;

	// create socket
	socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	std::cout << "This is the socket fd: " << socket_fd << "\n";
	if (socket_fd == -1)
		std::cout << "Failed to create socket.\n";

	port = 4242;
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = INADDR_ANY;
	socket_addr.sin_port = htons(port);
	// this will make addr reusable?!
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_addr, sizeof(socket_addr)) < 0)
		std::cout << "Failed to setsockopt.\n";

	// bind
	if (bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) < 0) {
		std::cout << "Failed to bind to port " << port << ". error: " << strerror(errno) << "\n";
		return (-1);
	}
	std::cout << "Bind to port " << port << " successful.\n";

	// listen
	if (listen(socket_fd, 1024)) {
		std::cout << "failed to listen on " << socket_fd << " \n";
		return (-1);
	}
	std::cout << "Listening on " << socket_fd << " \n";

	// try to set the socket non-blocking
	std::cout << "FD flag: " << fcntl(socket_fd, F_GETFD) << "\n";
	std::cout << "fd " << socket_fd;

	while(1){
		// accept connection
		accept_sock_fd = accept(socket_fd, (struct sockaddr*)&socket_addr, (unsigned int*)&socket_addr.sin_len);

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

	return (0);
};
