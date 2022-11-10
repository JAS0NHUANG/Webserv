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

struct Request {
	int 								method;
	std::string							path;
	bool								isHttp1_1;
	std::map<std::string, std::string>	headers;
	std::string							body;
};

// parseReq.cpp
void	parseRequest(std::string str, Request &req);

// main.cpp
void	*ft_memset(void *s, int c, size_t n);

#endif