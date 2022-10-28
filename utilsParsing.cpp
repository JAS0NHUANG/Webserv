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
	if (qu.front().size() == 0) {
		qu.pop();

		if (qu.size())
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

bool isOpenBracket(std::string &str) {
	return str == "{";
}

bool isCloseBracket(std::string &str) {
	return str == "}";
}

bool isSemicolon(std::string &str) {
	return str == ";";
}

bool isColon(std::string &str) {
	return str == ":";
}

std::string::size_type hasColon(std::string &str) {
	return str.find(':');
}

bool isFileEmpty(std::queue<std::vector<std::string> > &qu) {
	return qu.size() == 0;
}

void throwIfFileIsEmpty(std::string msg, std::queue<std::vector<std::string> > &qu) {
	if (isFileEmpty(qu))
		throw std::string(msg);
}

void throwParsingError( std::string token, std::string line, int reason, std::string bName, std::string fName) {
	static std::string binName = bName;
	static std::string fileName = fName;
	if (fName != "")
		return;
	if (reason == UNEXPECTED)
		throw std::string(binName + ": unexpected \"" + token + "\" in " + fileName + ":" + line);
	else if (reason == EXPECTED)
		throw std::string(binName + ": expected \"" + token + "\" in " + fileName + ":" + line);
}