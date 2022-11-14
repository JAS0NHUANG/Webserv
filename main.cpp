#include "request.hpp"
#include <sys/epoll.h>
#include <cstdlib>
#include <fcntl.h>

#define PORT_NUM	8080	// Notes: change it to port 80 (http)
#define BACKLOG		5		// Notes: change it to something bigger
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

std::string getRequest(int &connt_fd) {
	std::string request;
	char buffer[2056];

	ft_memset(buffer, 0, 2056);
	while (true) {
		int valread = recv(connt_fd, buffer, 2056, 0);
		request += buffer;
		ft_memset(buffer, 0, 2056);
		// std::cout << "valread : " << valread << "\n";
		if (valread == 1) // If we recall recv, it will block because EOF has been reached
			break;
		if (valread < 0)
			std::cerr << RED "recv failed\n" RESET;
	}

	return request;
}

std::string getResponse() {
	std::string response = "HTTP/1.1 200 OK\n"
		"Date: Thu, 19 Feb 2009 12:27:04 GMT\n"
		"Server: Apache/2.2.3\n"
		"Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
		"ETag: \"56d-9989200-1132c580\"\n"
		"Content-Type: text/html\n"
		"Content-Length: 17\n" // Size of the body
		"Accept-Ranges: bytes\n"
		"42Header: Jason/WenTsu/Stone\n"
		"\n"
		"<h1>HELLO!!!</h1>";
	return response;
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

int main()
{
	struct sockaddr_in sa;
	//struct sockaddr_in cli;
	int server_fd;
	//char str[100];

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		errMsgErrno("socket failed");
	else
		std::cout << BLU << "SOCKET OPENED\n" << RESET;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT_NUM);

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &sa, sizeof(sa)) < 0)
		errMsgErrno("setsockopt failed");
	if (bind(server_fd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
		errMsgErrno("BINDING FAILED");
	else
		std::cout << BLU << "BINDED SUCCESSFULLY\n" << RESET;

	if (listen(server_fd, BACKLOG) < 0)
		errMsgErrno("listen failed");

	std::cout << GRN ;
	std::cout << "LISTENING...\n";
	std::cout << RESET ;

    struct epoll_event ev, events[MAX_EVENTS];
    int nfds, epollfd;

	epollfd = epoll_create(1);
    if (epollfd == -1) {
		errMsgErrno("epoll failed");
		//exit(EXIT_FAILURE);
	}
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        errMsgErrno("epoll_ctl: listen_sock");
        //close(epollfd);
        //exit(EXIT_FAILURE);
    }

	while (true) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

		if (nfds == -1) {
            errMsgErrno("epoll_wait");
            //exit(EXIT_FAILURE);
        }
		for (int n = 0; n < nfds; ++n) {
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
                    errMsgErrno("fcntl set");
                    exit(EXIT_FAILURE);
                }
                add_event(epollfd, conn_sock, EPOLLIN);
                std::cout << "new connection:" << conn_sock << std::endl;
				//request here!!!!
            } else if(events[n].events & EPOLLIN){
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
				// send back the response
            } else if (events[n].events & EPOLLOUT){
                std::cout <<"write back"<< std::endl;
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
			    "<h1>reponse</h1>";

                if (send(events[n].data.fd, response.c_str(), response.size(), 0) < 0){
                   errMsgErrno("send error");
                }
                if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1)
                    errMsgErrno("EPOLL_CTL_DEL error2");
	            else
                    std::cout <<"one client delete"<< std::endl;
                close(events[n].data.fd);
            }
        }

		// socklen_t len = sizeof(cli);
		// int connt_fd = accept(server_fd, (struct sockaddr *)&cli, &len);
		// std::cout << GRN << "ACCEPTED\n" << RESET;
		// std::cout << "REQUEST FROM " <<inet_ntop(AF_INET, &cli.sin_addr, str, sizeof(str)) << ":" << htons(cli.sin_port) << "\n";

		// std::string request = getRequest(connt_fd);

		// std::cout << "Request:\n";
		// std::cout << YEL << request << RESET;

		// // NOTE : Parse request here
		// Request req;
		// // ft_memset(&req, 0, sizeof(req));
		// // req.headers.insert(std::make_pair("ASHE", "BOB"));

		// parseRequest(request, req);


		// // NOTE : Build response here

		// std::string response = getResponse();
		// // std::cout << "Response:\n";
		// // std::cout << YEL << response << RESET;

		// if (send(connt_fd, response.c_str(), response.size(), 0) < 0)
		// 	errMsgErrno("send failed");
		// std::cout << GRN "RESPONSE SENDED\n\n" RESET;
		// close(connt_fd);
	}
	close(epollfd);
	//close(server_fd);
}
