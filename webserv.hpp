#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <map>
#include <set>
#include <vector>
#include <queue>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "colorcodes.hpp"

// Classes
#include "Location.hpp"
class Server;
#include "Server.hpp"

#define UNEXPECTED 0
#define EXPECTED 1

enum directives {
	e_unknown,
	e_server,
	e_neutral,
	e_listen,
	e_server_name,
	e_allow_method,
	e_cgi,
	e_error_page,
	e_client_max_body_size,
	e_location,
	e_return,
	e_root,
	e_autoindex,
	e_index,
	e_upload_store
};

// saveFile.cpp
bool splitSemicolon(std::string &str, std::vector<std::string> &vec);
std::vector<std::string> splitLine(std::string str);
void deleteComment(std::vector<std::string> &vec);
void saveFile(char *fileName, std::queue<std::vector<std::string> >	&qu);

// configParsing.cpp
void callDoers(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void configParse(std::queue<std::vector<std::string> > &qu, std::vector<Server> &conf);
void debugPrintQ(std::queue<std::vector<std::string> >	&qu);
void parseFile(char *fileName, std::vector<Server> &conf);

// doParsing.cpp
void doServerParsing(std::queue<std::vector<std::string> > &qu, int &line);
void doListenParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line);
void doServerNameParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line);
void doErrorPageParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line);
void doClientMaxBodySizeParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line);
void doLocationParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line);
void doAllowMethodParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void doReturnParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void doRootParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void doAutoindexParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void doIndexParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void doCgiParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);
void doUploadStoreParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line);

// utilsParsing.cpp
void errMsgErrno(std::string msg);
void eraseToken(std::queue<std::vector<std::string> > &qu, int &line);
void checkIfLineIsEmpty(std::queue<std::vector<std::string> > &qu, int &line);
directives findDirective(std::string &str);
std::string toString(int n);
int toInt(std::string str);
unsigned long long toULL(std::string str);
bool isHttpErrorStatusCode(std::string str);
bool isRedirectStatusCode(std::string str);
void strToUpper(std::string &str);
bool isWhitespace(unsigned char c);
bool isNum(std::string str);
bool isValidMethod(std::string str);
std::string::iterator skipWhitespace(std::string &str, std::string::iterator it);
bool isServerDirective(std::string &str);
bool isDirective(std::string str);
bool isOpenBracket(std::string &str);
bool isCloseBracket(std::string &str);
bool isSemicolon(std::string &str);
bool isColon(std::string &str);
std::string::size_type hasColon(std::string &str);
bool isFileEmpty(std::queue<std::vector<std::string> > &qu);
void throwIfFileIsEmpty(std::string msg, std::queue<std::vector<std::string> > &qu);
void throwParsingError( std::string token, std::string line, int reason, std::string bName = "", std::string fName = "");

#endif