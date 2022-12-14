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
	std::cout << BBLU "New incoming connection:" << conn_sock << RESET "\n";
	return conn_sock;
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

	while (g_shutdown) {
		event_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

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
				clients[conn_sock] = new_client;
			}

			// Receiving request
			else if (events[n].events & EPOLLIN) {
				std::string raw_request  = clients[events[n].data.fd].recv_request();
				done = clients[events[n].data.fd].handle_request(raw_request);
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
				Response response(clients[events[n].data.fd]);
				done = response.send_response();
				if (done) {
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
