// #include <sys/socket.h>
// #include <iostream>

// #include "SimpleSocket.hpp"

// #include "BindingSocket.hpp"
// #include "ConnectingSocket.hpp"

// #include "ListeningSocket.hpp"

// #include "TestServer.hpp"

// #include <sys/event.h>
// #include <sys/types.h>

// int main(){
//     // std::cout << "starting ..." << std::endl;
//     // std::cout << "Binding ..." << std::endl;
//     // BindingSocket bs = BindingSocket(AF_INET, SOCK_STREAM, 0, 4241, INADDR_ANY);
//     // std::cout << "Listening ..." << std::endl;
//     // ListeningSocket ls = ListeningSocket(AF_INET, SOCK_STREAM, 0, 4243, INADDR_ANY, 20);
//     // std::cout << "done ..." << std::endl;

//     TestServer ts;


// }

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <string.h> // strerror
#include <errno.h> // errno
#include "colorcodes.h"
#include <unistd.h> //close
#include <fcntl.h>

#include <sys/event.h>
#include <sys/types.h>

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

    struct kevent evSet;
    struct	kevent evList[128];	 //Event triggered 
    int kq, nev;

    kq = kqueue();
    if ( kq == -1)
         errMsgErrno("kqueue failed"); 
    EV_SET(&evSet, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &evSet, 1, NULL, 0, NULL) < 0)
        errMsgErrno("kevent failed"); 
	while (1) {
		socklen_t addrlen;
		int new_socket;
        nev = kevent(kq, NULL, 0, evList, 128, NULL);
        if (nev < 0)
            errMsgErrno("kevent in loop failed"); 
        for (int i = 0; i < nev; i++) {
            int fd = (int)evList[i].ident;
            if (evList[i].flags & EV_EOF) {
                std::cout <<"Disconnect\n";
                close(fd);
                // Socket is automatically removed from the kq by the kernel.
            } else if (fd == server_fd) {
                struct sockaddr_storage addr;
                socklen_t socklen = sizeof(addr);
                int connfd = accept(fd, (struct sockaddr *)&addr, &socklen);
                if (connfd < 0)
                    errMsgErrno("accept failed");
                // Listen on the new socket
                EV_SET(&evSet, connfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                kevent(kq, &evSet, 1, NULL, 0, NULL);
	            std::cout << "new connection\n";
                int flags = fcntl(connfd, F_GETFL, 0);
                // if (flags < 0)
                //     errMsgErrno("GET fcntl");
                fcntl(connfd, F_SETFL, flags | O_NONBLOCK);
                // schedule to send the file when we can write (first chunk should happen immediately)
                // EV_SET(&evSet, connfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
                // kevent(kq, &evSet, 1, NULL, 0, NULL);
            } else if (evList[i].filter == EVFILT_READ) {
                char buf[1024] = {0};
                size_t bytes_read = recv(fd, buf, 1024, 0);
                std::cout << "READ : |"<< buf << "|"<< std::endl;
            } else if (evList[i].filter == EVFILT_WRITE) {
                std::cout<<"write :"<<std::endl;
                uintptr_t fd_client = evList[i].ident;
                char *s ="hello from server";
                //usleep(3000);
                if (int re = send(fd_client, s, 18, 0) != 0){
                    errMsgErrno("sendfile");
                    // if (errno == EAGAIN) {
                    // // schedule to send the rest of the file
                    // EV_SET(&evSet, fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, (void *)(offset + len));
                    // kevent(kq, &evSet, 1, NULL, 0, NULL);
                    // }
                }
            }
		// if ((new_socket = accept(server_fd, (struct sockaddr *) &address, &addrlen)) < 0)
		// 	errMsgErrno("accept failed");

		// std::cout << GRN ;
		// std::cout << "CONNECTION ACCEPTED\n";
		// std::cout << RESET ;

		// char buffer[1024] = {0};
		// int valread = recv(new_socket, buffer, 1024, 0);
		// std::cout << buffer;
		// if (valread < 0) {
		// 	std::cerr << RED ;
		// 	std::cerr << "recv failed\n";
		// 	std::cerr << RESET ;
		// }
		// std::string hello("Hello from webserv\n");
		// send(new_socket, hello.c_str(), hello.size(), 0);
        }
	}

	return 0;
}