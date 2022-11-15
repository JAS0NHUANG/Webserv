#include "request.hpp"

void debugPrintRequest(Request &req) {
	std::map<std::string, std::string>::iterator it;// = req.headers.begin();
	std::cout << MAG;
	std::cout << "Debug Request Structure:\n";
	std::cout << "Method    : " << req.method    << "\n";
	std::cout << "Path      : " << req.path      << "\n";
	std::cout << "Headers   : \n";
	for (it = req.headers.begin(); it != req.headers.end(); it++)
		std::cout << it->first << " : " << it->second << "\n";
	std::cout << "Body      : \n{|";
	std::cout << req.body << "|}\n";
	std::cout << RESET;
}

void   strToUpper(std::string &str) {
	std::string::iterator it = str.begin();
	for (; it != str.end(); it++)
		*it = std::toupper(*it);
}


bool isWhitespace(unsigned char c)
{
    if (c == ' ' || c == '\t' || c == '\n' ||
        c == '\r' || c == '\f' || c == '\v')
        return true;
    return false;
}

std::string::iterator skipWhitespace(std::string &str, std::string::iterator it) {
	while (isWhitespace(*it) && it != str.end())
		it++;
	return it;
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

std::string getNextString(std::string &line, std::string::iterator &it) {
	std::string str;
	for (; it != line.end() && !isWhitespace(*it); it++)
		str.push_back(*it);
	return str;
}

int methodToInt(std::string str) {
	if (str == "GET")
		return GET;
	if (str == "POST")
		return POST;
	return DELETE;
}

bool isValidMethod(std::string str) {
	return (str == "GET" || str == "POST" || str == "DELETE");
}