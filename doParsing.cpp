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

void doListenParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line) {
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

void doServerNameParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line) {
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

void doErrorPageParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPY", qu);

	std::vector<std::string> strvec;
	while (qu.size() && !isSemicolon(qu.front().front())) {
		strvec.push_back(qu.front().front());
		eraseToken(qu, line);
		throwIfFileIsEmpty("EMPTY", qu);
	}

	if (strvec.size() < 2) {
		std::cerr << "error_page directive need a minimum of 2 arguments\n";
		// NOTE : need to throw here
		return;
	}

	std::string uri = strvec.back();
	strvec.pop_back();

	std::vector<int> codes;
	std::vector<std::string>::iterator it = strvec.begin();
	for (; it != strvec.end(); it++) {
		if (isNum(*it) == false) {
			std::cerr << "NOT INT\n";
			// NOTE : need to throw here
			return;
		}
		// NOTE : need to check if error_code is valid
		codes.push_back(toInt(*it));
	}

	conf.set_error_page(codes, uri);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doClientMaxBodySizeParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (isNum(qu.front().front()) == false) {
		std::cerr << "NOT NUM\n";
		// NOTE : need to throw here
		return;
	}
	conf.set_client_max_body_size(toULL(qu.front().front()));
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

int methodToInt(std::string str) {
	if (str == "GET")
		return GET;
	if (str == "POST")
		return POST;
	return DELETE;
}



void doAllowMethodParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	// NOTE: need to check method properly (if they are valid)
	// Also 1 method supported for need to support more
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	std::vector<int> vec;
	while (qu.size() && !isSemicolon(qu.front().front())) {
		strToUpper(qu.front().front());
		if (isValidMethod(qu.front().front()) == false) {
			std::cerr << "NOT A VALID METHOD\n";
			// NOTE : need to throw here
			return;
		}
		vec.push_back(methodToInt(qu.front().front()));
		eraseToken(qu, line);
		throwIfFileIsEmpty("EMPTY", qu);
	}

	conf.set_allow_method(vec);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doReturnParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	// NOTE: Check if its an url of a text
	// For this will always that its a text

	// NOTE : This below is not well supported (need to fix it)
	// return code;
	// return code [text];
	// return code URL;
	// return URL;
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (isNum(qu.front().front()) == false) {
		std::cerr << "NOT NUMMMMM\n";
		// NOTE : need to throw here
		return;
	}
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

void doRootParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_root(qu.front().front());
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doAutoindexParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	strToUpper(qu.front().front());
	if (qu.front().front() != "ON" && qu.front().front() != "OFF") {
		std::cerr << "NOT ON || OFF\n";
		// NOTE : need to throw here
		return;
	}

	if (qu.front().front() == "ON")
		conf.set_autoindex(true);
	else
		conf.set_autoindex(false);

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doIndexParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
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

void doCgiParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
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

void doUploadStoreParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_upload_store(qu.front().front());

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.size() == 0 || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doLocationParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line) {
	// NOTE: Maybe we need to check if it's a valid uri
	// NOTE: How we can consider a valid uri ?

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY !", qu);

	std::string uri = qu.front().front();
	Location location(conf);

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

	if (isFileEmpty(qu) || qu.front().front() != "}")
		throwParsingError("}", toString(line), EXPECTED);
		// add : expected '}' to close location directive
	
	eraseToken(qu, line);
}