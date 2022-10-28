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

#define PORT_NUM	4242	// Notes: change it to port 80 (http)
#define BACKLOG		5		// Notes: change it to something bigger
#define EPOLLEVENTS 100
#define MAX_EVENTS 128
#define BUFFER_SIZE 1024

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

int	recv_event(int fd)
{
	int	ret = 0;
	int	len;
	std::string str;

	while (1)
	{
		char	buf[BUFFER_SIZE];
		len = recv(fd, buf, BUFFER_SIZE - 1, 0);
		if (len <= 0)
			break ;
		ret += len;
		buf[len] = 0;
		str += buf;
	}
    std::cout << "READ : |"<< str << "|" << std::endl;
	return ret;
}

static void add_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1){
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

    epollfd = epoll_create(1);
    if (epollfd == -1) {
		errMsgErrno("epoll failed");
		exit(EXIT_FAILURE);
	}
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        errMsgErrno("epoll_ctl: listen_sock");
        //close(epollfd);
        //exit(EXIT_FAILURE);
    }
	while (1) {
		socklen_t addrlen;
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            errMsgErrno("epoll_wait");
            //exit(EXIT_FAILURE);
        }
        for (int n = 0; n < nfds; ++n) {
            std::cout << "n :" << n << " nfds :" << nfds << std::endl;
            if (events[n].events & EPOLLRDHUP || events[n].events & EPOLLERR || events[n].events & EPOLLHUP){
                errMsgErrno("event error");
                continue;
            }
            else if (events[n].data.fd == server_fd){
                struct sockaddr_storage addr;
                socklen_t socklen = sizeof(addr);
                int conn_sock = accept(server_fd, (struct sockaddr *)&addr, &socklen);
                if (conn_sock < 0){
                    errMsgErrno("accept failed");
                    continue;
                }
                int flag = fcntl(conn_sock,F_GETFL,0);
                if (fcntl(conn_sock, F_SETFL, flag | O_NONBLOCK) < 0){
                   // errMsgErrno("fcntl set");
                    exit(EXIT_FAILURE);
                }
                //setnonblocking(conn_sock);
                add_event(epollfd, conn_sock, EPOLLIN);
                std::cout << "new connection:" << conn_sock << std::endl;
            } else if(events[n].events & EPOLLIN){
                // if (recv_event(events[n].data.fd) < 0)
                //     errMsgErrno("recv error");
                // if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1)
                //     errMsgErrno("EPOLL_CTL_DEL error");
	            // else
                //     std::cout <<"one client delete"<< std::endl;
                int ret = recv_event(events[n].data.fd);
                if (ret > 0){
                    ev.events = EPOLLOUT;
                    ev.data.fd = events[n].data.fd;
                    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd, &ev) == -1)
                        errMsgErrno("EPOLL_CTL_MOD error");
                } else if (ret == 0){
                    std::cout <<"client close"<< std::endl;
                    close(events[n].data.fd);
                    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1)
                        errMsgErrno("EPOLL_CTL_DEL error1");
	                else
                        std::cout <<"one client delete"<< std::endl;
                } else{
                    errMsgErrno("recv error");
                }
            } else if (events[n].events & EPOLLOUT){
                std::cout <<"write back"<< std::endl;
                // const char *msg = "HTTP/1.1 200 OK \r\n\r\n<h1>hello world</h1><r><n>";
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
                if (int re = send(events[n].data.fd, response.c_str(), response.size(), 0) < 0){
                    errMsgErrno("send error");
                }
                if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1)
                    errMsgErrno("EPOLL_CTL_DEL error2");
	            else
                    std::cout <<"one client delete"<< std::endl;
                close(events[n].data.fd);
            }
        }
	}
    close(epollfd);
    //close(server_fd);??
	return 0;
}