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

void splitLine(std::string str, std::vector<std::string> &vec) {
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
}

void deleteComment(std::vector<std::string> &vec) {
	std::vector<std::string>::iterator	it;
	std::string::size_type				i;

	for (it = vec.begin(); it != vec.end(); it++) {
		i = (*it).find('#');
		if (i != std::string::npos) {
			(*it).resize(i);
			break;
		}
	}

	if (it++ != vec.end())
		vec.erase(it, vec.end());
}

void parseFile(char *fileName, ServerConf &conf)
{
	(void)conf;
	std::ifstream				configFile(fileName);
	std::vector<std::string>	vec;

	if (!configFile.is_open())
	{
		std::cerr << RED;
		std::cerr << "Unable to open configuration file\n";
		std::cerr << RESET;
		return;
	}

	std::string line;
	while (getline(configFile, line)) {
		std::cout << "Initial line: |" << line << "|\n";
		splitLine(line, vec);
		deleteComment(vec);
		for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
			std::cout << "str : |" << *it << "|\n";
		// ...
		vec.clear();
	}

	configFile.close();
}