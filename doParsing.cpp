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
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	std::string address, port;
	std::string str = qu.front().front();
	std::string::size_type i = 0;

	i = str.find(":", i);
	if (i == std::string::npos) {
		std::cerr << "LISTEN BAD ARGUMENTS\n";
		// NOTE : need to throw here
		return;
	}
	address = str.substr(0, i);
	std::cout << "address : " << address << "\n";

	port = str.substr(i + 1, str.size() - i);
	std::cout << "port    : " << port << "\n";

	if (address == "localhost")
		conf.set_address("127.0.0.1");
	else if (address == "*")
		conf.set_address("0.0.0.0");
	else if (isValidIpAddress(address))
		conf.set_address(address);
	else {
		std::cerr << "IP address is invalid\n";
		// NOTE : need to throw here
		return;
	}

	if (!isNum(port) || !isValidPort(port)) {
		std::cerr << "Port number is invalid\n";
		//NOTE : need to throw here
		return ;
	}
	conf.set_port(toInt(port));

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.empty() || qu.front().front() != ";")
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

	if (qu.empty())
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
		if (isHttpErrorStatusCode(*it) == false) {
			std::cerr << "NOT A VALID HTTP STATUS CODE\n";
			// NOTE : need to throw here
			return;
		}
		codes.push_back(toInt(*it));
	}

	conf.set_error_page(codes, uri);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doClientMaxBodySizeParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	std::string::iterator it = qu.front().front().end() - 1;
	if (*it != 'm' && *it != 'M') {
		std::cerr << "expected 'm' to precise memory capacity unit\n";
		// NOTE : need to throw here
		return;
	}
	qu.front().front().erase(it);


	if (isNum(qu.front().front()) == false) {
		std::cerr << "NOT NUM\n";
		// NOTE : need to throw here
		return;
	}
	conf.set_client_max_body_size(toULL(qu.front().front()));
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.empty() || qu.front().front() != ";")
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

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doReturnParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_return(qu.front().front());
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doRootParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_root(qu.front().front());
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.empty() || qu.front().front() != ";")
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

	if (qu.empty() || qu.front().front() != ";")
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

	if (qu.empty() || qu.front().front() != ";")
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

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doUploadStoreParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	conf.set_upload_store(qu.front().front());

	eraseToken(qu, line);
	throwIfFileIsEmpty("EMPTY", qu);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError(";", toString(line), EXPECTED);

	eraseToken(qu, line);
}

void doLocationParsing(std::queue<std::vector<std::string> > &qu, Server &conf, int &line) {
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