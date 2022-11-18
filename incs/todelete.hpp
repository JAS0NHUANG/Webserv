#ifndef TODELETE_HPP
#define TODELETE_HPP

#include "webserv.hpp"

struct Request {
	int 								method;
	std::string							path;
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
void getStringAsQueue(std::string &str, std::queue<std::string> &qu);
void debugPrintQ(std::queue<std::string> &qu);
void skipQueueEmptyLines(std::queue<std::string> &qu);
std::string getNextString(std::string &line, std::string::iterator &it);

#endif