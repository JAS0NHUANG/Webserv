#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h> 

#include "../../incs/webserv.hpp"

#define PORT_NUM	4242	// Notes: change it to port 80 (http)
#define BACKLOG		5		// Notes: change it to something bigger
#define EPOLLEVENTS 100
#define MAX_EVENTS 128

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

static void add_event(int epollfd,int fd,int state){
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		throwError("epoll_ctl");
}

static int check_event_fd(int event_fd, std::vector<Socket> &socket_list) {
	for(int i = 0;i < (int)socket_list.size();i++) {
		if (event_fd == socket_list[i].getSockFd())
			return (1);
	}
	return (0);
}

int accept_conn(struct epoll_event ev, int epollfd) {
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);
	int conn_sock = accept(ev.data.fd, (struct sockaddr *)&addr, &socklen);
	if (conn_sock < 0)
		errMsgErrno("accept");
	int flag = fcntl(conn_sock, F_GETFL, 0);
	if (fcntl(conn_sock, F_SETFL, flag | O_NONBLOCK) < 0)
		errMsgErrno("fcntl");
	add_event(epollfd, conn_sock, EPOLLIN);
	std::cout << "New incoming connection:" << conn_sock << "\n";
	return conn_sock;
}

int run_epoll(std::vector<Socket> &socket_list) {

	struct epoll_event ev, events[MAX_EVENTS];
	int event_fds, epollfd;
	std::map<int, Client> clients;
	bool done;

	// create the epoll instance
	epollfd = epoll_create(1);
	if (epollfd == -1)
		throwError("epoll_create");
	for(int i = 0;i < (int)socket_list.size();i++) {
		std::cout << "socket fd: " << socket_list[i].getSockFd() << "\n";
		add_event(epollfd, socket_list[i].getSockFd(), EPOLLIN);
	}

	while (true) {
		event_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

		if (event_fds == -1)
			errMsgErrno("epoll_wait");

		// Loop that handle events happening on server fd and connections fds
		for (int n = 0; n < event_fds; ++n) {
			std::cout << MAG <<  "n :" << n << " event_fds :" << event_fds << std::endl << RESET; 
			std::cout << MAG << "n :" << n << " current event fd :" << events[n].data.fd << std::endl << RESET;
			if (events[n].events & EPOLLRDHUP || events[n].events & EPOLLERR || events[n].events & EPOLLHUP){
				errMsgErrno("event error");
				continue;
			}
			// Accepting a new connection
			else if (check_event_fd(events[n].data.fd, socket_list)) {
				int conn_sock = accept_conn(events[n], epollfd);
				Client new_client(conn_sock);
				clients[conn_sock] = new_client;
			}
			// Receiving request
			else if (events[n].events & EPOLLIN) {
				done = clients[events[n].data.fd].recv_request();

				if (done) {
					ev.events = EPOLLOUT;
					ev.data.fd = events[n].data.fd;
					if (epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd, &ev) == -1)
						errMsgErrno("epoll_ctl (op: EPOLL_CTL_MOD)");
				}
			}
			// Sending response
			else if (events[n].events & EPOLLOUT) {
				std::cout << "Creating a response\n";
				done = clients[events[n].data.fd].send_response();

				if (done) { // <-- NOTE : TO FIX
					clients.erase(events[n].data.fd);
					if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1)
						errMsgErrno("epoll_ctl (op: EPOLL_CTL_DEL)");
					else
						std::cout << "Client deleted from event list\n";

					if (close(events[n].data.fd) < 0)
						errMsgErrno("close");
					std::cout << "A connection has been closed\n";
				}
			}
		}
	}
	if (close(epollfd) == -1)
		errMsgErrno("close");
	return 0;
}
