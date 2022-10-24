#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <string>
#include <fstream>
#include <iostream>
#include "colorcodes.hpp"

// Classes
#include "BaseConf.hpp"
#include "ServerConf.hpp"

// configParsing-utils.cpp
bool isWhitespace(unsigned char c);
std::string::iterator	skipWhitespace(std::string &str, std::string::iterator it);
std::vector<std::string> splitLine(std::string str);
std::vector<std::string> deleteComment(std::vector<std::string> vec);

#endif