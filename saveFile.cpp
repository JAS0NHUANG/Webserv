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

std::string::size_type smallestIndex(std::vector<std::string::size_type> stvec) {
	std::string::size_type i = stvec.back();
	stvec.pop_back();
	while (!stvec.empty()) {
		if (i > stvec.back())
			i = stvec.back();
		stvec.pop_back();
	}
	return i;
}

std::vector<std::string> splitTokens(std::string str, std::vector<std::string> &strVec) {
	std::string::size_type i = 0;
	std::vector<std::string::size_type> indexVec;
	std::string tmp;

	while (!str.empty()) {
		indexVec.clear();
		indexVec.push_back( str.find("{"));
		indexVec.push_back( str.find("#"));
		indexVec.push_back( str.find("}"));
		indexVec.push_back( str.find(";"));

		i = smallestIndex(indexVec);
		if (i == std::string::npos) {
			strVec.push_back(str);
			return strVec;
		}

		if (i != std::string::npos) {
			tmp = str.substr(0, i);
			if (!tmp.empty())
				strVec.push_back(tmp);

			if (str[i] == '{')
				strVec.push_back("{");
			else if (str[i] == '}')
				strVec.push_back("}");
			else if (str[i] == '#')
				strVec.push_back("#");
			else if (str[i] == ';')
				strVec.push_back(";");

			str.erase(0, i + 1);
		}
	}

	return strVec;
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
		if (!tmp.empty())
			splitTokens(tmp, vec);

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
