#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h> 

#include "../../incs/webserv.hpp"

#define PORT_NUM	4242	// Notes: change it to port 80 (http)
#define BACKLOG		5		// Notes: change it to something bigger
#define EPOLLEVENTS 100
#define MAX_EVENTS 128
#define BUFFER_SIZE 1024


static int	recv_event(int fd)
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

int	create_epoll() {
	return (0);
}

int run_epoll(std::vector<Socket> &socket_list) {
	std::cout << GRN ;
	std::cout << "socket_fd: " << socket_list[0].getSockFd() ;
	std::cout << "LISTENING...\n";
	std::cout << "LISTENING...\n";
	std::cout << RESET ;

	int server_fd = socket_list[0].getSockFd();
	struct epoll_event ev, events[MAX_EVENTS];
	int event_fds, epollfd;

	// create the epoll instance
	epollfd = epoll_create(1);
	if (epollfd == -1) {
		errMsgErrno("epoll failed");
		exit(EXIT_FAILURE);
	}
	ev.events = EPOLLIN;
	ev.data.fd = socket_list[0].getSockFd();
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
		errMsgErrno("epoll_ctl: listen_sock");
		//close(epollfd);
		//exit(EXIT_FAILURE);
	}
	int trytry = 0;
	while (1) {
		event_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

		if (event_fds == -1) {
			errMsgErrno("epoll_wait");
			//exit(EXIT_FAILURE);
		}
		for (int n = 0; n < event_fds; ++n) {
			std::cout << "n :" << n << " event_fds :" << event_fds << std::endl;
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
				//setnonblocking(conn_sock);
				int flag = fcntl(conn_sock,F_GETFL,0);
				if (fcntl(conn_sock, F_SETFL, flag | O_NONBLOCK) < 0){
					// errMsgErrno("fcntl set");
					exit(EXIT_FAILURE);
				}
				add_event(epollfd, conn_sock, EPOLLIN);
				std::cout << "new connection:" << conn_sock << std::endl;
			} else if(events[n].events & EPOLLIN){
				// if (recv_event(events[n].data.fd) < 0)
				// errMsgErrno("recv error");
				// if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1)
				// errMsgErrno("EPOLL_CTL_DEL error");
				// else
				// std::cout <<"one client delete"<< std::endl;
				int recv_char = recv_event(events[n].data.fd);
				if (recv_char > 0){
					ev.events = EPOLLOUT;
					ev.data.fd = events[n].data.fd;
					if(recv_char > 10)
						trytry = 1;
					else
						trytry = -1;
					if (epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd, &ev) == -1)
						errMsgErrno("EPOLL_CTL_MOD error");
					usleep(1000);
				} else if (recv_char == 0){
					std::cout <<"client close"<< std::endl;
					close(events[n].data.fd);
					if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1)
						errMsgErrno("EPOLL_CTL_DEL error1");
					else
						std::cout <<"one client delete"<< std::endl;
				} else {
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
					"<h1>reponse 1</h1>";
				std::string response1 =
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
					"<h1>reponse 2</h1>";
				//if (int re = send(events[n].data.fd, response.c_str(), response.size(), 0) < 0){
					//errMsgErrno("send error");
				//}
				if (trytry == 1)
					send(events[n].data.fd, response.c_str(), response.size(), 0);
				else if (trytry == -1)
					send(events[n].data.fd, response1.c_str(), response1.size(), 0);
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
