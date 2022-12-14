#include "webserv.hpp"

std::string::size_type smallest_index(std::vector<std::string::size_type> stvec) {
	std::string::size_type i = stvec.back();
	stvec.pop_back();
	while (!stvec.empty()) {
		if (i > stvec.back())
			i = stvec.back();
		stvec.pop_back();
	}
	return i;
}

std::vector<std::string> split_tokens(std::string str, std::vector<std::string> &strVec) {
	std::string::size_type i = 0;
	std::vector<std::string::size_type> indexVec;
	std::string tmp;

	while (!str.empty()) {
		indexVec.clear();
		indexVec.push_back( str.find("{"));
		indexVec.push_back( str.find("#"));
		indexVec.push_back( str.find("}"));
		indexVec.push_back( str.find(";"));

		i = smallest_index(indexVec);
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

std::vector<std::string> split_line(std::string str) {
	std::vector<std::string> 	vec;
	std::string::iterator		itStart;
	std::string::iterator		itEnd;

	if (str.size() == 0)
		return vec;
	for (itStart = str.begin(); itStart < str.end(); itStart++) {
		itStart = skip_whitespace(str, itStart);
		itEnd = itStart;

		for (; itEnd < str.end() && !is_whitespace(*itEnd); itEnd++) {}

		std::string tmp(itStart, itEnd);
		if (!tmp.empty())
			split_tokens(tmp, vec);

		itStart = itEnd;
	}

	delete_comment(vec);

	return vec;
}

void delete_comment(std::vector<std::string> &vec) {
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

void save_file(char *fileName, std::queue<std::vector<std::string> >	&qu) {
	std::ifstream	configFile(fileName);

	if (!configFile.is_open())
		throw std::string("Unable to open configuration file: " + std::string(fileName));

	std::string line;
	while (getline(configFile, line)) {
		std::vector<std::string> vec = split_line(line);
		qu.push(vec);
	}
	configFile.close();
}
