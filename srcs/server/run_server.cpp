#include "webserv.hpp"
#include "Response.hpp"

static void add_event(int epollfd,int fd,int state){
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		throwError("epoll_ctl");
}

static std::vector<Socket>::iterator check_event_fd(int event_fd, std::vector<Socket> &socket_list) {
	std::vector<Socket>::iterator it = socket_list.begin();
	for (; it != socket_list.end(); it++)
		if (event_fd == (*it).getSockFd())
			break;
	return it;
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
	return conn_sock;
}

void check_client_timeouts(std::map<int, Client> &clients, int &epollfd,
	struct epoll_event ev, struct epoll_event events[]) {
	std::map<int, Client>::iterator it = clients.begin();

	for (; it != clients.end(); it++) {
		if (it->second.get_timeout() > 30) {
			ev.events = EPOLLOUT;
			ev.data.fd = events[it->first].data.fd;
			if (epoll_ctl(epollfd, EPOLL_CTL_MOD, events[it->first].data.fd, &ev) == -1)
				errMsgErrno("epoll_ctl (op: EPOLL_CTL_MOD)");
			it->second.set_timeout_status_code();
		}
	}
}

int run_server(std::vector<Socket> &socket_list) {

	struct epoll_event ev, events[MAX_EVENTS];
	int event_fds, epollfd;
	std::map<int, Client> clients;
	bool done;

	// create the epoll instance
	epollfd = epoll_create(1);
	if (epollfd == -1)
		throwError("epoll_create");
	for(int i = 0;i < (int)socket_list.size();i++) {
		add_event(epollfd, socket_list[i].getSockFd(), EPOLLIN);
	}

	std::cout << BGRN << UGRN << ">> Webserv successfully running and waiting for requests <<" << RESET << std::endl;
	while (g_shutdown) {
		event_fds = epoll_wait(epollfd, events, MAX_EVENTS, 5000); // blocking 5 secs

		if (event_fds == -1)
			errMsgErrno("epoll_wait");

		// Loop that handle events happening on server fd and connections fds
		for (int n = 0; n < event_fds; ++n) {

			if (events[n].events & EPOLLRDHUP || events[n].events & EPOLLERR || events[n].events & EPOLLHUP) {
				errMsgErrno("event error");
				continue;
			}

			// Accepting a new connection
			std::vector<Socket>::iterator it = check_event_fd(events[n].data.fd, socket_list);
			if (it != socket_list.end()) {
				int conn_sock = accept_conn(events[n], epollfd);
				Client new_client(conn_sock, (*it).get_virtual_servers());
				clients[n] = new_client;
			}

			// Receiving request
			else if (events[n].events & EPOLLIN) {
				clients[n].recv_request();
				done = clients[n].handle_request();
				if (done) {
					std::cout << YEL"done request!!!\n" << RESET;
					ev.events = EPOLLOUT;
					ev.data.fd = events[n].data.fd;
					if (epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd, &ev) == -1)
						errMsgErrno("epoll_ctl (op: EPOLL_CTL_MOD)");
				}
				else
					clients[events[n].data.fd].start_timeout();
			}

			// Sending response
			else if (events[n].events & EPOLLOUT) {
				std::cout << YEL "responding!!\n" RESET;
				Response response(clients[n]);
				done = response.send_response();
				if (done) {
					clients.erase(n);
					if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1)
						errMsgErrno("epoll_ctl (op: EPOLL_CTL_DEL)");
					else

					if (close(events[n].data.fd) < 0)
						errMsgErrno("close");
				}
			}
		}
		check_client_timeouts(clients, epollfd, ev, events);
	}
	if (close(epollfd) == -1)
		errMsgErrno("close");
	return 0;
}
