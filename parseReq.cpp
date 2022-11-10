#include "request.hpp"

void saveRequestLine(std::queue<std::string> &qu, Request &req) {
	if (!qu.empty()) {
		std::string::iterator it = qu.front().begin();
		it = skipWhitespace(qu.front(), it);

		std::string str = getNextString(qu.front(), it);
		if (isValidMethod(str))
			req.method = methodToInt(str);
		else
			std::cout << RED << "INVALID METHOD\n" << RESET;

		it = skipWhitespace(qu.front(), it);
		str = getNextString(qu.front(), it);
		if (!str.empty())
			req.path = str;
		else
			std::cout << RED << "INVALID PATH\n" << RESET;

		it = skipWhitespace(qu.front(), it);
		str = getNextString(qu.front(), it);
		if (!str.empty() && str == "HTTP/1.1")
			req.isHttp1_1 = true;
		else
			std::cout << RED << "INVALID HTTP VERSION\n" << RESET;

		it = skipWhitespace(qu.front(), it);
		if (it != qu.front().end())
			std::cout << RED << "MALFORMED REQUEST LINE\n" << RESET;
		// std::cout << "req line : " << qu.front() << "\n";
		qu.pop();
	}
	else
		std::cout << RED "ERROR NO REQUEST LINE\n" RESET;
}

void saveHeaders(std::queue<std::string> &qu, Request &req) {
	std::string::iterator it;
	std::string::size_type i = std::string::npos;
	std::pair<std::string, std::string> header;
	while (!qu.empty() && !qu.front().empty() && qu.front() != "\r") {
		i = qu.front().find(':');
		if (i != std::string::npos){
			header.first = qu.front().substr(0, i);
			qu.front().erase(0, i + 1);
		}
		else
			std::cout << RED << "MALFORMED HEADER\n" << RESET;

		it = qu.front().begin();
		it = skipWhitespace(qu.front(), it);
		header.second = getNextString(qu.front(), it);
		if (!header.second.empty() && *it == '\r')
			req.headers.insert(header);
		else
			std::cout << RED << "MALFORMED HEADER\n" << RESET;

		qu.pop();
	}
}

void saveBody(std::queue<std::string> &qu, Request &req) {
	while (!qu.empty()) {
		req.body += qu.front() + "\n";
		qu.pop();
	}
}

void parseRequest(std::string str, Request &req) {
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
	saveHeaders(qu, req);

	// 4.
	saveBody(qu, req);

	// Debugging Request Structure
	debugPrintRequest(req);

	std::cout << MAG << "Done Parsing Request...\n" << RESET;
}