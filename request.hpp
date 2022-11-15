#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>	// Temporary required for inet_ntop()
#include <string.h>		// strerror
#include <errno.h>		// errno
#include <unistd.h>		// close()
#include <sys/epoll.h>
#include <cstdlib>
#include <fcntl.h>

#include <cstddef>
#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include "colorcodes.hpp"

#define GET		1
#define POST	2
#define DELETE	3

#define PORT_NUM 8080	// NOTE : To be set with the compliance of the config file
#define BACKLOG 1000 // Number of client that can connect to the server simultaneously
#define MAX_EVENTS 128	
#define BUFFER_SIZE 2056

struct Request {
	int 								method;
	std::string							path;
	std::map<std::string, std::string>	headers;
	std::string							body;
};

struct Webserv {
	struct sockaddr_in sa;
	int server_fd;
	struct epoll_event ev, events[MAX_EVENTS];
	int nfds, epollfd;
};

// parseReq.cpp
void parseRequest(std::string str, Request &req);
void saveRequestLine(std::queue<std::string> &qu, Request &req);
void saveHeaders(std::queue<std::string> &qu, Request &req);
void saveBody(std::queue<std::string> &qu, Request &req);

// utilsParseReq.cpp
void debugPrintRequest(Request &req);
bool isWhitespace(unsigned char c);
std::string::iterator skipWhitespace(std::string &str, std::string::iterator it);
void getStringAsQueue(std::string &str, std::queue<std::string> &qu);
void debugPrintQ(std::queue<std::string> &qu);
void skipQueueEmptyLines(std::queue<std::string> &qu);
std::string getNextString(std::string &line, std::string::iterator &it);
int methodToInt(std::string str);
bool isValidMethod(std::string str);
void strToUpper(std::string &str);

// main.cpp
void	*ft_memset(void *s, int c, size_t n);
void errMsgErrno(std::string msg);

// initWebserv.cpp
void initWebserv(Webserv &datas);

#endif