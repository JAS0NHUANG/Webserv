#include "req-resp.hpp"

void	debugPrintRequest(Request &req) {
	std::map<std::string, std::string>::iterator it;// = req.headers.begin();
	std::cout << MAG;
	std::cout << "Debug Request Structure:\n";
	std::cout << "Method    : " << req.method    << "\n";
	std::cout << "Path      : " << req.path      << "\n";
	std::cout << "isHttp1.1 : " << req.isHttp1_1 << "\n";

	// NOTE : Why am I segfaulting here ??!
	// std::cout << "Headers   : \n";
	// for (it = req.headers.begin(); it != req.headers.end(); it++)
	// 	std::cout << it->first << " : " << it->second << "\n";

	std::cout << "Body      : \n";
	std::cout << req.body << "\n";
	std::cout << RESET;
}

void getStringAsQueue(std::string &str, std::queue<std::string> &qu) {
	std::string line;
	std::istringstream iss(str);

	while (getline(iss, line))
		qu.push(line);
}

void debugPrintQ(std::queue<std::string> &qu) {
	while (!qu.empty()) {
		std::cout << BYEL << qu.front() << "\n";
		qu.pop();
	}
}

void skipQueueEmptyLines(std::queue<std::string> &qu) {
	while (!qu.empty() && qu.front().empty())
		qu.pop();
}

void saveRequestLine(std::queue<std::string> &qu, Request &req) {
	(void)req; //
	if (!qu.empty()) {
		// TODO
		std::cout << "req line : " << qu.front() << "\n";
		qu.pop();
	}
	else
		std::cout << RED "Error : no request line\n" RESET;
}

void saveHeaders(std::queue<std::string> &qu, Request &req) {
	(void)req; //
	while (!qu.empty() && !qu.front().empty()) {
		// TODO
		qu.pop();
	}
}

void saveBody(std::queue<std::string> &qu, Request &req) {
	while (!qu.empty()) {
		req.body += qu.front() + "\n";
		qu.pop();
	}
}

void	parseRequest(std::string str, Request &req) {
	ft_memset(&req, 0, sizeof(req));
	std::cout << MAG << "Parsing Request...\n" << RESET;

	std::queue<std::string> qu;
	getStringAsQueue(str, qu);

	// Careful, if debug is used, qu will be empty at the end
	// debugPrintQ(qu);

	// NOTE : HTTP/1.1 must have a 'Host' header
	// 1 - Skip empty lines (need to verify it's really needed...)
	// 2 - Retrieve request line info (method, path and http version)
	// 3 - While there is not an empty line, save headers pairs
	// 4 - Save body if there is one

	// 1.
	skipQueueEmptyLines(qu);

	// 2.
	saveRequestLine(qu, req);

	// 3.
	saveBody(qu, req);

	// Debugging Request Structure
	debugPrintRequest(req);

	std::cout << MAG << "Done Parsing Request...\n" << RESET;
}