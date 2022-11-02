#include "webserv.hpp"

void errMsgErrno(std::string msg) {
	std::cerr << RED ;
	std::cerr << msg << "\n";
	std::cerr << strerror(errno) << "\n";
	std::cerr << RESET ;
}

void eraseToken(std::queue<std::vector<std::string> > &qu, int &line) {
	qu.front().erase(qu.front().begin());
	checkIfLineIsEmpty(qu, line);
}

void checkIfLineIsEmpty(std::queue<std::vector<std::string> > &qu, int &line) {
	// Skip empty lines
	while (!qu.empty() && qu.front().empty()) {
		qu.pop();
		if (!qu.empty())
			++line;
	}
}

directives findDirective(std::string &str) {
	if (str == "listen")
		return e_listen;
	else if (str == "server_name")
		return e_server_name;
	else if (str == "error_page")
		return e_error_page;
	else if (str == "client_max_body_size")
		return e_client_max_body_size;
	else if (str == "location")
		return e_location;
	else if (str == "allow_method")
		return e_allow_method;
	else if (str == "return")
		return e_return;
	else if (str == "root")
		return e_root;
	else if (str == "autoindex")
		return e_autoindex;
	else if (str == "index")
		return e_index;
	else if (str == "cgi")
		return e_cgi;
	else if (str == "upload_store")
		return e_upload_store;
	return e_unknown;
}

std::string toString(int n) {
	std::string			str;
	std::stringstream	ss;

	ss << n;
	ss >> str;

	return str;
}

int toInt(std::string str) {
	int					n;
	std::stringstream	ss;

	ss << str;
	ss >> n;

	return n;
}

unsigned long long toULL(std::string str) {
	unsigned long long	n;
	std::stringstream	ss;

	ss << str;
	ss >> n;

	return n;
}

void	strToUpper(std::string &str) {
	std::string::iterator it = str.begin();
	for (; it != str.end(); it++)
		*it = std::toupper(*it);
}

int methodToInt(std::string str) {
	if (str == "GET")
		return GET;
	if (str == "POST")
		return POST;
	return DELETE;
}

bool isValidIpAddress(std::string str) {
	std::string sub;
	int loop = 0;
	std::string::size_type j = 0;
	std::string::size_type i = 0;

	for (; loop != 4; loop++) {
		if (loop != 3)
			i = str.find(".", i + 1);
		if (i != std::string::npos) {
			sub = str.substr(j, i - j);
			if (sub.empty() || !isNum(sub) || sub.size() > 3 || !(toInt(sub) >= 0 && toInt(sub) <= 255))
				return false;
		}
		else
			return false;
		j = i + 1;
	}
	return true;
}

bool isValidPort(std::string str) {
	if (toInt(str) >= 0 && toInt(str) <= 65535)
		return true;

	return false;
}

bool isWhitespace(unsigned char c)
{
    if (c == ' ' || c == '\t' || c == '\n' ||
        c == '\r' || c == '\f' || c == '\v')
        return true;
    return false;
}

std::string::iterator	skipWhitespace(std::string &str, std::string::iterator it) {
	while (isWhitespace(*it) && it != str.end())
		it++;
	return it;
}

bool isNum(std::string str) {
	std::string::iterator it = str.begin();
	for (; it != str.end(); it++)
		if (std::isdigit(*it) == false)
			return false;

	return true;
}

bool isHttpErrorStatusCode(std::string str) {
	std::string codes[40] = {"400", "401", "402", "403", "404", "405", "406",
		"407", "408", "409", "410", "411", "412", "413", "414", "415", "416",
		"417", "418", "421", "422", "423", "424", "425", "426", "428", "429",
		"431", "451", "500", "501", "502", "503", "504", "505", "506", "507",
		"508", "510", "511"};

	for (int i = 0; i != 40; i++)
		if (str == codes[i])
			return true;

	return false;
}

bool isRedirectStatusCode(std::string str) {
	std::string codes[7] = {"300", "301", "302", "303", "304", "307", "308"};

	for (int i = 0; i != 7; i++)
		if (str == codes[i])
			return true;

	return false;
}

bool isValidMethod(std::string str) {
	return (str == "GET" || str == "POST" || str == "DELETE");
}

bool isServerDirective(std::string &str) {
	return str == "server";
}

bool isDirective(std::string str) {
	if (str == "listen" || str == "server_name" || str == "error_page"
	|| str == "client_max_body_size" || str == "location" || str == "allow_method"
	|| str == "return" || str == "root" || str == "autoindex"
	|| str == "index" || str == "cgi" || str == "upload_store")
		return true;
	return false;
}

void throwIfFileIsEmpty(std::queue<std::vector<std::string> > &qu, int line) {
	if (qu.empty())
		throwParsingError("unexpected end of file", toString(line));
}

void throwParsingError( std::string msg, std::string line, std::string bName, std::string fName) {
	static std::string binName = bName;
	static std::string fileName = fName;
	if (fName != "")
		return;

	throw std::string(binName + ": " + msg + " in " + fileName + ":" + line);
}