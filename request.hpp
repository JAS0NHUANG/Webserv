#ifndef REQRES_HPP
#define REQRES_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>	// Temporary required for inet_ntop()
#include <string.h>		// strerror
#include <errno.h>		// errno
#include <unistd.h>		// close()

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

struct Request {
	int 								method;
	std::string							path;
	bool								isHttp1_1;
	std::map<std::string, std::string>	headers;
	std::string							body;
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

// main.cpp
void	*ft_memset(void *s, int c, size_t n);

#endif