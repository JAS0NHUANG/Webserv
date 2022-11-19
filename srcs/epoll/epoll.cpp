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

bool getRequest(int connt_fd, std::map<int, std::string> &requests) {
	char buffer[BUFFER_SIZE] = {0};
	static int i = 1; // for test purpose
	if (requests.count(connt_fd) == 0)
		requests.insert(std::pair<int, std::string>(connt_fd, ""));
	std::string request = requests.find(connt_fd)->second;
	while (true) {
		int valread = recv(connt_fd, buffer, BUFFER_SIZE - 1, 0);
		if (valread == 0) {
			requests[connt_fd] = request;
			return true;
		}
		else if (valread < 0) {
			if (errno == EAGAIN)
				break;
			errMsgErrno("recv");
		}
		buffer[valread] = '\0';
		request += buffer;
	}
	requests[connt_fd] = request;
	std::cout << YEL "READ : \n|" << request << "|\n" RESET;
	// Indeed we need to parse the request as we 
	// receiving it, that means that as soon as possible
	// we will respond to client

	if (i == 4) // for test purpose
		return true;
	i++;
	return false;
}

static void add_event(int epollfd,int fd,int state){
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1){
		errMsgErrno("epoll ctl");
		//exit(EXIT_FAILURE);
	}
}

static int check_event_fd(int event_fd, std::vector<Socket> &socket_list) {
	for(int i = 0;i < (int)socket_list.size();i++) {
		if (event_fd == socket_list[i].getSockFd())
			return (1);
	}
	return (0);
}

int	epoll_init() {
	return (0);
}

void sendResponse(std::string &response, struct epoll_event event) {
	if (send(event.data.fd, response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
}

std::pair<bool, std::string> handleRequest(std::string &request) {
	(void)request;
	// Maybe we will use struct Request instead of the string request
	std::pair<bool, std::string> resp;

	std::cout << "Handling request\n";
	resp.first = true;
	resp.second = "HTTP/1.1 200 OK\n\nHello world\n";
	return resp;
}

Request accept_conn(struct epoll_event ev, int epollfd) {
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);
	int conn_sock = accept(ev.data.fd, (struct sockaddr *)&addr, &socklen);
	if (conn_sock < 0){
		errMsgErrno("accept");
		// return;
	}
	int flag = fcntl(conn_sock, F_GETFL, 0);
	if (fcntl(conn_sock, F_SETFL, flag | O_NONBLOCK) < 0) {
		errMsgErrno("fcntl");
		exit(EXIT_FAILURE);
	}
	add_event(epollfd, conn_sock, EPOLLIN);
	std::cout << "New incoming connection:" << conn_sock << "\n";
	return Request(conn_sock);
}

int run_epoll(std::vector<Socket> &socket_list) {

	struct epoll_event ev, events[MAX_EVENTS];
	int event_fds, epollfd;
	// Request req;
	std::vector<Request> vecreq;

	// [ fd | request ] if the request has not been totally read, the fd stays in this structure.
	// Maybe we will need to add a pair <string (request), time> to know 
	// if the timeout of receiving a request has been reached.
	std::map<int, std::string> requests;

	// create the epoll instance
	epollfd = epoll_create(1);
	if (epollfd == -1) {
		errMsgErrno("epoll");
		exit(EXIT_FAILURE);
	}
	for(int i = 0;i < (int)socket_list.size();i++) {
		std::cout << "socket fd: " << socket_list[i].getSockFd() << "\n";
		add_event(epollfd, socket_list[i].getSockFd(), EPOLLIN);
	}

	while (true) {
		event_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

		if (event_fds == -1) {
			errMsgErrno("epoll_wait");
			// exit(EXIT_FAILURE);
		}

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
				Request req(accept_conn(events[n], epollfd));
				vecreq.push_back(req);
			}
			// Receiving request
			else if (events[n].events & EPOLLIN) {
				vecreq[0].recv_buffer();
				// bool requestIsComplete = getRequest(events[n].data.fd, requests);
				// if (requestIsComplete) {
				if (vecreq[0].is_complete()) {
					ev.events = EPOLLOUT;
					ev.data.fd = events[n].data.fd;
					if (epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd, &ev) == -1)
						errMsgErrno("epoll_ctl, EPOLL_CTL_MOD");
				}
			}
			// Sending response
			else if (events[n].events & EPOLLOUT) {
				std::cout << "Creating a response\n";

				// [ true = close , false = stay open | request string ]
				std::pair<bool, std::string> resp;

				// handleRequest will create the response as a pair,
				// the boolean says if we close the connection,
				// and the string is the response
				resp = handleRequest(requests.find(events->data.fd)->second);
				std::cout << YEL << "Response :\n|" << resp.second << "|\n" RESET;
				requests.erase(events->data.fd);
				sendResponse(resp.second, events[n]);

				// We close if the connection is no more needed
				// But for that we need to parse the request
				// For now by default we will always close 
				// a connection when the request has been successfully received
				if (resp.first == true) {
					if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1)
						errMsgErrno("epoll_ctl, EPOLL_CTL_DEL");
					else
						std::cout << "Client deleted from event list\n";

					if (close(events[n].data.fd) < 0)
						errMsgErrno("close");
					else
						std::cout << "A connection has been closed\n";
				}
			}
		}
	}
	close(epollfd);
	return 0;
}
