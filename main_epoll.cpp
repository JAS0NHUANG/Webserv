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

#include <sys/epoll.h> 

#define PORT_NUM	50000	// Notes: change it to port 80 (http)
#define BACKLOG		5		// Notes: change it to something bigger
#define EPOLLEVENTS 100
#define MAX_EVENTS 10

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

static void add_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev == -1){
        errMsgErrno("epoll ctl failed");
        //exit(EXIT_FAILURE);
    }
}
// clang++ -Wall -Wextra -Werror -std=c++98 main.cpp -o server && ./server
int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	/* The original socket that was set up
	for listening is used only for accepting
	connections, not for exchanging data.*/
	int	server_fd = -1;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		errMsgErrno("socket failed");
        exit(EXIT_FAILURE);
	}

	struct sockaddr_in address;

	ft_memset((char *) &address, 0, (sizeof(address)));
	address.sin_family		= AF_INET;
	address.sin_addr.s_addr	= htonl(INADDR_ANY);
	address.sin_port		= htons(PORT_NUM);

	// This set the socket able to be reused immediately .
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &address, sizeof(address)) < 0){
		errMsgErrno("setsockopt failed");
        exit(EXIT_FAILURE);
	}

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0){
		errMsgErrno("bind failed");
        exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 1) < 0){
		errMsgErrno("listen failed");
    	exit(EXIT_FAILURE);
	}

	std::cout << GRN ;
	std::cout << "LISTENING...\n";
	std::cout << RESET ;

    struct epoll_event ev, events[MAX_EVENTS];
    int conn_sock, nfds, epollfd;

    epollfd = epoll_create(1));
    if (epoll_fd == -1) {
		errMsgErrno("epoll failed");
		exit(EXIT_FAILURE);
	}
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        errMsgErrno("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
	while (1) {
		socklen_t addrlen;
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            errMsgErrno("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (int n = 0; n < nfds; ++n) {
            std::cout << "n :" << n << " nfds :" << nfds << std::endl;
            if (events[n].events & POLLPRI || events[n].events & EPOLLRDHUP || events[n].events & EPOLLERR || events[n].events & EPOLLHUP){
                errMsgErrno("event error");
                continue;
            }
            else if (events[n].data.fd == server_fd){
                struct sockaddr_storage addr;
                socklen_t socklen = sizeof(addr);
                int conn_sock = accept(fd, (struct sockaddr *)&addr, &socklen);
                if (conn_sock < 0){
                    errMsgErrno("accept failed");
                    continue;
                }
                if (fcntl(conn_sock, F_SETFL, flags | O_NONBLOCK) < 0){
                    errMsgErrno("fcntl set");
                    exit(EXIT_FAILURE);
                }
                //setnonblocking(conn_sock);
                add_event(epoll_fd, conn_sock, EPOLLIN);
                std::cout << "new connection:" << conn_sock << std::endl;
            } else if(events[n].events & EPOLLIN){
                char buf[1024] = {0};
                size_t bytes_read = recv(fd, buf, 1024, 0);
                std::cout << "READ : |"<< buf << "|"<< std::endl;
                // if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn_sock, NULL) == -1)
		        //     print_log("EPOLL_CTL_DEL error");
	            // else
		        //     print_log("One event delete");
            } else if (events[n].events & EPOLLOUT){
                std::cout << "write\n";
                char s[] ="hello from server";
                //usleep(3000);
                if (int re = send(events[n].data.fd, s, 18, 0) != 0){
                    errMsgErrno("send error");
                }
            }
        }
	}
    close(epoll_fd);
    close(server_fd);
	return 0;
}