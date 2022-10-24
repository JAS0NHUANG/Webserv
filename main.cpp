#include "colorcodes.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include "ServerConf.hpp"

// References:
// - https://www.linode.com/docs/guides/how-to-configure-nginx/
// - https://www.digitalocean.com/community/tools/nginx
// - https://www.plesk.com/blog/various/nginx-configuration-guide/
// - http://kbeezie.com/nginx-configuration-examples/
// - https://doc.ubuntu-fr.org/nginx
// - https://subscription.packtpub.com/book/networking-and-servers/9781785288654/1/ch01lvl1sec09/testing-nginx-configuration
// - https://docs.nginx.com/nginx/admin-guide/web-server/web-server/

void parseFile(char *fileName)
{
	std::ifstream configFile(fileName);
	if (!configFile.is_open())
	{
		std::cerr << RED;
		std::cerr << "Unable to open configuration file\n";
		std::cerr << RESET;
		return;
	}

	std::string line;
	while (getline(configFile, line))
		std::cout << line;

	configFile.close();
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

std::vector<std::string> splitStr(std::string str) {
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

int main(int argc, char *argv[])
{
	ServerConf conf;

	std::vector<std::string> vec(splitStr("Ceci   \t est \t\tune\tstring\r   de     test."));
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		std::cout << "tmp = |" << *it << "|\n";

	// conf.debug();

	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " configuration_file\n";
	}
	else
		parseFile(argv[1]);

	return 0;
}