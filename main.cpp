#include "request.hpp"

void *ft_memset(void *s, int c, size_t n)
{
	while (n)
	{
		*((unsigned char *)s + n - 1) = (unsigned char)c;
		n--;
	}
	return (s);
}

void errMsgErrno(std::string msg)
{
	std::cerr << RED;
	std::cerr << msg << "\n";
	std::cerr << strerror(errno) << "\n";
	std::cerr << RESET;
}

static void add_event(int epollfd, int fd, int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		errMsgErrno("epoll ctl failed");
}

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
			errMsgErrno("recv failed");
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

std::pair<bool, std::string> handleRequest(std::string &request) {
	(void)request;
	// Maybe we will use struct Request instead of the string request
	std::pair<bool, std::string> resp;

	std::cout << "Handling request\n";
	resp.first = true;
	resp.second = "HTTP/1.1 200 OK\n\nHello world\n";
	return resp;
}

void sendResponse(std::string &response, struct epoll_event event) {
	if (send(event.data.fd, response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
}

int main()
{
	Webserv datas;
	initWebserv(datas);

	std::map<int, std::string> requests;
	// [ fd | request ] if the request has not been totally read, the fd stays in this structure.
	// Maybe we will need to add a pair <string (request), time> to know 
	// if the timeout of receiving a request has been reached.

	// Main loop that runs the webserv without end
	while (true)
	{
		datas.nfds = epoll_wait(datas.epollfd, datas.events, MAX_EVENTS, -1);

		if (datas.nfds == -1) {
			errMsgErrno("epoll_wait failed");
			// exit(EXIT_FAILURE);
		}

		// Loop that handle events happening on server fd and connections fds
		for (int n = 0; n < datas.nfds; ++n) {
			if (datas.events[n].events & EPOLLRDHUP ||
				datas.events[n].events & EPOLLERR ||
				datas.events[n].events & EPOLLHUP) {
				errMsgErrno("event failed");
				continue;
			}
			// Accepting a new connection
			else if (datas.events[n].data.fd == datas.server_fd) {
				struct sockaddr_storage addr;
				socklen_t socklen = sizeof(addr);
				int conn_sock = accept(datas.server_fd, (struct sockaddr *)&addr, &socklen);
				if (conn_sock < 0) {
					errMsgErrno("accept failed");
					continue;
				}
				int flag = fcntl(conn_sock, F_GETFL, 0);
				if (fcntl(conn_sock, F_SETFL, flag | O_NONBLOCK) < 0) {
					errMsgErrno("fcntl failed");
					exit(EXIT_FAILURE);
				}
				add_event(datas.epollfd, conn_sock, EPOLLIN);
				std::cout << "New incoming connection:" << conn_sock << "\n";
			}
			// Receiving request
			else if (datas.events[n].events & EPOLLIN) {
				bool requestIsComplete = getRequest(datas.events[n].data.fd, requests);
				if (requestIsComplete) {
					datas.ev.events = EPOLLOUT;
					datas.ev.data.fd = datas.events[n].data.fd;
					if (epoll_ctl(datas.epollfd, EPOLL_CTL_MOD, datas.events[n].data.fd, &datas.ev) == -1)
						errMsgErrno("epoll_ctl, EPOLL_CTL_MOD failed");
				}
			}
			// Sending response
			else if (datas.events[n].events & EPOLLOUT) {
				std::cout << "Creating a response\n";

				// [ true = close , else false | request string ]
				std::pair<bool, std::string> resp;

				// handleRequest will create the response as a pair,
				// the boolean says if we close the connection,
				// and the string is the response
				resp = handleRequest(requests.find(datas.events->data.fd)->second);
				std::cout << YEL << "Response :\n|" << resp.second << "|\n" RESET;
				requests.erase(datas.events->data.fd);
				sendResponse(resp.second, datas.events[n]);

				// We close if the connection is no more needed
				// But for that we need to parse the request
				// For now by default we will always close 
				// a connection when the request has been successfully received
				if (resp.first == true) {
					if (epoll_ctl(datas.epollfd, EPOLL_CTL_DEL, datas.events[n].data.fd, NULL) == -1)
						errMsgErrno("epoll_ctl, EPOLL_CTL_DEL failed");
					else
						std::cout << "Client deleted from event list\n";

					if (close(datas.events[n].data.fd) < 0)
						errMsgErrno("close failed");
					else
						std::cout << "A connection has been closed\n";
				}
			}
		}
	}
	close(datas.epollfd);
}
