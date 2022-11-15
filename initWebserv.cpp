#include "request.hpp"

void initWebserv(Webserv &datas) {
	if ((datas.server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		errMsgErrno("socket failed");
	else
		std::cout << BLU << "SOCKET OPENED\n"
				  << RESET;

	ft_memset(&datas.sa, 0, sizeof(datas.sa));
	datas.sa.sin_family = AF_INET;
	datas.sa.sin_port = htons(PORT_NUM);

	if (setsockopt(datas.server_fd, SOL_SOCKET, SO_REUSEADDR, &datas.sa, sizeof(datas.sa)) < 0)
		errMsgErrno("setsockopt failed");

	if (bind(datas.server_fd, (struct sockaddr *)&datas.sa, sizeof(datas.sa)) < 0)
		errMsgErrno("BINDING FAILED");
	else
		std::cout << BLU << "BINDED SUCCESSFULLY\n"
				  << RESET;

	if (listen(datas.server_fd, BACKLOG) < 0)
		errMsgErrno("listen failed");
	else
		std::cout << GRN "LISTENING...\n" RESET;

	datas.epollfd = epoll_create(1);
	if (datas.epollfd == -1)
		errMsgErrno("epoll failed");

	datas.ev.events = EPOLLIN;
	datas.ev.data.fd = datas.server_fd;
	if (epoll_ctl(datas.epollfd, EPOLL_CTL_ADD, datas.server_fd, &datas.ev) == -1)
		errMsgErrno("epoll_ctl: listen_sock");
}