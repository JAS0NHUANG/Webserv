#include "webserv.hpp"

void doServerParsing(std::queue<std::vector<std::string> > &qu, int &line) {
	if (qu.front().front() != "server")
		throwParsingError(qu.front().front(), toString(line), UNEXPECTED);
		// add: " Expected "server" "

	eraseToken(qu, line);

	if (!isOpenBracket(qu.front().front()))
		throwParsingError(qu.front().front(), toString(line), UNEXPECTED);
		//add: " Expected "{" "

	eraseToken(qu, line);
	
}

void doListenParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	// NOTE: IMCOMPLETE -> only handle: listen port;
	//			need to handle: 		listen address:port; etc...
	std::vector<std::string> vec;

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	vec.push_back(qu.front().front());
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_port(toInt(vec.front()));

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doServerNameParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	std::vector<std::string> names;
	while (qu.size() && !isSemicolon(qu.front().front())) {
		names.push_back(qu.front().front());
		eraseToken(qu, line);
		throwIfFileIsEmpty("EMPTY", qu);
	}

	conf.set_server_name(names);

	if (qu.size() == 0)
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doErrorPageParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {

	// NOTE: IMCOMPLETE, only handle one code with one path...
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPY", qu);

	std::vector<int>	codes;
	std::string			path;

	codes.push_back(toInt(qu.front().front()));
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	path = qu.front().front();
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	// while (qu.size() && !isSemicolon(qu.front()[0])) {
	// 	eraseToken(qu, line);
	// }

	conf.set_error_page(codes, path);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doClientMaxBodySizeParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	// NOTE: CHeck if its a real number.
	conf.set_client_max_body_size(toULL(qu.front().front()));
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doAllowMethodParsing(std::queue<std::vector<std::string> > &qu, BaseConf &conf, int &line) {
	// NOTE: need to check method properly (if they are valid)
	// Also 1 method supported for need to support more
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	std::vector<int> vec;
	vec.push_back(toInt(qu.front().front()));

	conf.set_allow_method(vec);
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doReturnParsing(std::queue<std::vector<std::string> > &qu, BaseConf &conf, int &line) {
	// NOTE: Check if its an url of a text
	// For this will always that its a text

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	int code = toInt(qu.front().front());
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	std::string str = qu.front().front();
	int type = IS_TEXT;
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_return(code, str, type);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doRootParsing(std::queue<std::vector<std::string> > &qu, BaseConf &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_root(qu.front().front());
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doAutoindexParsing(std::queue<std::vector<std::string> > &qu, BaseConf &conf, int &line) {
	// NOTE: Check the value of the token: if its 'on' or 'off'
	// For now it will always return on

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_autoindex(true);
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doIndexParsing(std::queue<std::vector<std::string> > &qu, BaseConf &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	std::vector<std::string> vec;
	while (qu.size() && !isSemicolon(qu.front().front())) {
		vec.push_back(qu.front().front());
		eraseToken(qu, line);
		throwIfFileIsEmpty("EMPTY", qu);
	}

	conf.set_index(vec);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doCgiParsing(std::queue<std::vector<std::string> > &qu, BaseConf &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	std::string ext = qu.front().front();
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	std::string path = qu.front().front();
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_cgi(ext, path);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doUploadStoreParsing(std::queue<std::vector<std::string> > &qu, BaseConf &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_upload_store(qu.front().front());

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doLocationParsing(std::queue<std::vector<std::string> > &qu, ServerConf &conf, int &line) {
	// NOTE: Maybe we need to check if it's a valid uri

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY !", qu);

	std::string uri = qu.front().front();
	BaseConf location;

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY !", qu);

	if (!isOpenBracket(qu.front().front()))
		throwParsingError(qu.front().front(), toString(line), UNEXPECTED);
		// add : expected '{'

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY !", qu);


	while (qu.size() && !isCloseBracket(qu.front().front()))
		callDoers(qu, location, line);

	conf.set_location(uri, location);

	if (isFileEmpty(qu))
		throwParsingError("}", toString(line), EXPECTED);
		// add : expected '}' to close location directive
	
	eraseToken(qu, line);
}