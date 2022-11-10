#include "request.hpp"

#define PORT_NUM	4242	// Notes: change it to port 80 (http)
#define BACKLOG		5		// Notes: change it to something bigger

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

int main()
{
	struct sockaddr_in sa;
	struct sockaddr_in cli;
	int server_fd;
	char str[100];

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

	while (true) {
		socklen_t len = sizeof(cli);
		int connt_fd = accept(server_fd, (struct sockaddr *)&cli, &len);
		std::cout << GRN << "ACCEPTED\n" << RESET;
		std::cout << "REQUEST FROM " <<inet_ntop(AF_INET, &cli.sin_addr, str, sizeof(str)) << ":" << htons(cli.sin_port) << "\n";

		std::string request = getRequest(connt_fd);

		std::cout << "Request:\n";
		std::cout << YEL << request << RESET;

		// NOTE : Parse request here
		Request req;
		// ft_memset(&req, 0, sizeof(req));
		// req.headers.insert(std::make_pair("ASHE", "BOB"));

		parseRequest(request, req);


		// NOTE : Build response here

		std::string response = getResponse();
		// std::cout << "Response:\n";
		// std::cout << YEL << response << RESET;

		if (send(connt_fd, response.c_str(), response.size(), 0) < 0)
			errMsgErrno("send failed");
		std::cout << GRN "RESPONSE SENDED\n\n" RESET;
		close(connt_fd);
	}
}
