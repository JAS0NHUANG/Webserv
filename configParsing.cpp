#include "webserv.hpp"

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

std::vector<std::string> splitLine(std::string str) {
	std::vector<std::string>	vec;
	std::string::iterator		itStart;
	std::string::iterator		itEnd;

	for (itStart = str.begin(); itStart < str.end(); itStart++) {
		itStart = skipWhitespace(str, itStart);
		itEnd = itStart;

		for (; itEnd < str.end() && !isWhitespace(*itEnd); itEnd++) {}

		std::string tmp(itStart, itEnd);
		vec.push_back(tmp);

		itStart = itEnd;
	}
	return vec;
}

std::vector<std::string> deleteComment(std::vector<std::string> vec) {
	std::vector<std::string>::iterator	it;
	std::string::size_type				i;

	for (it = vec.begin(); it != vec.end(); it++) {
		i = (*it).find('#');
		if (i != std::string::npos) {
			std::cout << "Before resizing: |" << *it << "|\n";
			(*it).resize(i);
			std::cout << "After resizing: |" << *it << "|\n";
			break;
		}
	}
	if (it++ != vec.end())
		vec.erase(it, vec.end());
	return vec;
}