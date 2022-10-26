#include "webserv.hpp"

bool splitSemicolon(std::string &str, std::vector<std::string> &vec) {
	std::string::size_type	i = str.find(';');
	std::string::iterator	it;
	std::string				tmp;

	if (i == std::string::npos)
		return false;
	it = str.begin();

	tmp.assign(it, it + i);

	if (tmp.size())
		vec.push_back(tmp);
	vec.push_back(";");
	it = it + i + 1;
	if (it == str.end())
		return true; 
	tmp.assign(it, str.end());
	vec.push_back(tmp);
	return true;

}

std::vector<std::string> splitLine(std::string str) {
	std::vector<std::string> 	vec;
	std::string::iterator		itStart;
	std::string::iterator		itEnd;

	if (str.size() == 0)
		return vec;
	for (itStart = str.begin(); itStart < str.end(); itStart++) {
		itStart = skipWhitespace(str, itStart);
		itEnd = itStart;

		for (; itEnd < str.end() && !isWhitespace(*itEnd); itEnd++) {}

		std::string tmp(itStart, itEnd);
		if (tmp.size() && !splitSemicolon(tmp, vec)) {
			vec.push_back(tmp);
		}

		itStart = itEnd;
	}

	deleteComment(vec);

	return vec;
}

void deleteComment(std::vector<std::string> &vec) {
	std::vector<std::string>::iterator	it;
	std::string::size_type				i;

	for (it = vec.begin(); it != vec.end(); it++) {
		i = (*it).find('#');
		if (i != std::string::npos) {
			if (i != 0) {
				(*it).resize(i);
				it++;
			}
			break;
		}
	}

	if (it != vec.end())
		vec.erase(it, vec.end());
}

void saveFile(char *fileName, std::queue<std::vector<std::string> >	&qu) {
	std::ifstream	configFile(fileName);

	if (!configFile.is_open())
		throw std::string("Unable to open configuration file: " + std::string(fileName));

	std::string line;
	while (getline(configFile, line)) {
		std::vector<std::string> vec = splitLine(line);
		if (vec.size())
			qu.push(vec);
	}
	configFile.close();
}
