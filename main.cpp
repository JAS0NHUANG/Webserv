#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <string.h> // strerror
#include <errno.h> // errno
#include "colorcodes.h"

#define PORT_NUM	50000	// Notes: change it to port 80 (http)
#define BACKLOG		5		// Notes: change it to something bigger

void	*ft_memset(void *s, int c, size_t n)
{
	while (n)
	{
		*((unsigned char *)s + n - 1) = (unsigned char)c;
		n--;
	}
	return (s);
}

void	errMsgErrno(std::string msg) {
	std::cerr << RED ;
	std::cerr << msg << "\n";
	std::cerr << strerror(errno) << "\n";
	std::cerr << RESET ;
}

// clang++ -Wall -Wextra -Werror -std=c++98 main.cpp -o server && ./server
int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	/* The original socket that was set up
	for listening is used only for accepting
	connections, not for exchanging data.*/
	int	server_fd = -1;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		errMsgErrno("socket failed");

	struct sockaddr_in address;

	ft_memset((char *) &address, 0, (sizeof(address)));
	address.sin_family		= AF_INET;
	address.sin_addr.s_addr	= htonl(INADDR_ANY);
	address.sin_port		= htons(PORT_NUM);

	// This set the socket able to be reused immediately .
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &address, sizeof(address)) < 0)
		errMsgErrno("setsockopt failed");

	if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0)
		errMsgErrno("bind failed");

	if (listen(server_fd, 1) < 0)
		errMsgErrno("listen failed");

	std::cout << GRN ;
	std::cout << "LISTENING...\n";
	std::cout << RESET ;

	while (1) {
		socklen_t addrlen;
		int new_socket;
		if ((new_socket = accept(server_fd, (struct sockaddr *) &address, &addrlen)) < 0)
			errMsgErrno("accept failed");

		std::cout << GRN ;
		std::cout << "CONNECTION ACCEPTED\n";
		std::cout << RESET ;

		char buffer[1024] = {0};
		int valread = recv(new_socket, buffer, 1024, 0);
		std::cout << buffer;
		if (valread < 0) {
			std::cerr << RED ;
			std::cerr << "recv failed\n";
			std::cerr << RESET ;
		}

		std::string hello("Hello from webserv\n");
		send(new_socket, hello.c_str(), hello.size(), 0);
	}

	return 0;
}