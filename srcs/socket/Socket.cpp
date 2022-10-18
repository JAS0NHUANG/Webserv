#include "../../incs/Socket.hpp"

int	create_socket(void) {
	int	port;
	sockaddr_in	socket_addr;

	int	socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	int	accept_sock_fd;
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
		if (listen(socket_fd, 1024)) {
			std::cout << "failed to listen on " << socket_fd << " \n";
		} else {
			std::cout << "Listening on " << socket_fd << " \n";
		}
		accept_sock_fd = accept(socket_fd, (struct sockaddr*)&socket_addr, (unsigned int*)&socket_addr.sin_len);

		// Read from the connection
  char buffer[100];
  read(accept_sock_fd, buffer, 100);
  std::cout << "The message was: " << buffer;

  // Send a message to the connection
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

  // Close the connections

/*
		std::cout << "Accept connection on " << accept_sock_fd << " \n";
		write(accept_sock_fd, "HTTP/1.1 404 Page No Found\nServer: darkautism web server\nContent-Type: text/html;charset=utf-8", 105);
		*/
	}

	return (0);
};
