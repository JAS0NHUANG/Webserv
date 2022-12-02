#include "webserv.hpp"

void doServerParsing(std::queue<std::vector<std::string> > &qu, int &line) {
	if (qu.front().front() != "server")
		throwParsingError(std::string("unexpected '" + qu.front().front() + "'"), toString(line));

	eraseToken(qu, line);

	if (qu.front().front() != "{")
		throwParsingError("expected '{'", toString(line));

	eraseToken(qu, line);
}

void doListenParsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	std::string address, port;
	std::string str = qu.front().front();
	std::string::size_type i = 0;

	i = str.find(":", i);
	if (i == std::string::npos)
		throwParsingError("listen directive bad arguments", toString(line));

	address = str.substr(0, i);
	port = str.substr(i + 1, str.size() - i);

	if (address == "localhost")
		conf.set_address("127.0.0.1");
	else if (address == "*")
		conf.set_address("0.0.0.0");
	else if (isValidIpAddress(address))
		conf.set_address(address);
	else {
		throwParsingError("invalid ip address", toString(line));
		return;
	}

	if (!isNum(port) || !isValidPort(port)) {
		throwParsingError("invalid port number", toString(line));
		return ;
	}
	conf.set_port(toInt(port));

	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError("expected ';'", toString(line));

	eraseToken(qu, line);
}

void doServerNameParsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	std::vector<std::string> names;
	std::cout << RED "START DEBUG\n" RESET;
	while (qu.size() && qu.front().front() != ";") {
		if (qu.front().front() == "}" || qu.front().front() == "{")
			throwParsingError("expected ';'", toString(line));
		std::cout << "name : " << qu.front().front() << "\n";
		names.push_back(qu.front().front());
		eraseToken(qu, line);
		throwIfFileIsEmpty(qu, line);
	}
	std::cout << RED "END DEBUG\n" RESET;

	conf.set_server_name(names);

	if (qu.empty())
		throwParsingError("expected ';'", toString(line));

	eraseToken(qu, line);
}


void doErrorPageParsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	std::vector<std::string> strvec;
	while (qu.size() && qu.front().front() != ";") {
		strvec.push_back(qu.front().front());
		eraseToken(qu, line);
		throwIfFileIsEmpty(qu, line);
	}

	if (strvec.size() < 2)
		throwParsingError("error_page directive need a minimum of 2 arguments", toString(line));

	std::string uri = strvec.back();
	strvec.pop_back();

	std::vector<int> codes;
	std::vector<std::string>::iterator it = strvec.begin();
	for (; it != strvec.end(); it++) {
		if (isNum(*it) == false)
			throwParsingError(std::string(*it + " is not an integer"), toString(line));

		if (isHttpErrorStatusCode(*it) == false)
			throwParsingError(std::string(*it + " is not a valid http status code"), toString(line));

		codes.push_back(toInt(*it));
	}

	conf.set_error_page(codes, uri);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError("expected ';;", toString(line));

	eraseToken(qu, line);
}

void doClientMaxBodySizeParsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	std::string::iterator it = qu.front().front().end() - 1;
	if (*it != 'm' && *it != 'M')
		throwParsingError("expected 'm' to precise memory capacity unit", toString(line));

	qu.front().front().erase(it);


	if (isNum(qu.front().front()) == false)
		throwParsingError(std::string(qu.front().front() + " is not an integer"), toString(line));

	conf.set_client_max_body_size(toULL(qu.front().front()) * 1000000);
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError("expected ';'", toString(line));

	eraseToken(qu, line);
}

void doAllowMethodParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	std::vector<std::string> vec;
	while (qu.size() && qu.front().front() != ";") {
		strToUpper(qu.front().front());
		if (isValidMethod(qu.front().front()) == false)
			throwParsingError(std::string(qu.front().front() + " is not a valid method"), toString(line));

		vec.push_back(qu.front().front());
		eraseToken(qu, line);
		throwIfFileIsEmpty(qu, line);
	}

	conf.set_allow_method(vec);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError("expected ';'", toString(line));

	eraseToken(qu, line);
}

void doReturnParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	if (!qu.empty() && (qu.front().front().find_first_not_of("0123456789") == std::string::npos)) {
		conf.set_return_status(qu.front().front());
		eraseToken(qu, line);
		throwIfFileIsEmpty(qu, line);
	}
	conf.set_return(qu.front().front());
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError("expected ';'", toString(line));

	eraseToken(qu, line);
}

void doRootParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	if ((qu.front().front().size() > 1) && *(qu.front().front().end() - 1) == '/')
		qu.front().front().erase(qu.front().front().size() - 1);

	conf.set_root(qu.front().front());
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError("expected ';'", toString(line));

	eraseToken(qu, line);
}

void doAutoindexParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	strToUpper(qu.front().front());
	if (qu.front().front() != "ON" && qu.front().front() != "OFF")
		throwParsingError("expected 'on' of 'off'", toString(line));

	if (qu.front().front() == "ON")
		conf.set_autoindex(true);
	else
		conf.set_autoindex(false);

	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError("expected ';'", toString(line));

	eraseToken(qu, line);
}

void doIndexParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	std::vector<std::string> vec;
	while (qu.size() && qu.front().front() != ";") {
		vec.push_back(qu.front().front());
		eraseToken(qu, line);
		throwIfFileIsEmpty(qu, line);
	}

	conf.set_index(vec);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError("expected ';'", toString(line));

	eraseToken(qu, line);
}

void doCgiParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	std::string ext = qu.front().front();

	if (ext[0] != '.')
		ext = "." + ext;

	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	std::string path = qu.front().front();
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	conf.set_cgi(ext, path);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError("expected ';'", toString(line));

	eraseToken(qu, line);
}

void doUploadStoreParsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	conf.set_upload_store(qu.front().front());

	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throwParsingError("expected ';'", toString(line));

	eraseToken(qu, line);
}

void doLocationParsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line) {
	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	std::string uri = qu.front().front();

	if ((uri.size() > 1) && *(uri.end() - 1) == '/')
		uri.erase(uri.size() - 1);

	Location location(conf);

	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	if (qu.front().front() != "{")
		throwParsingError(std::string("unexpected '" + qu.front().front() + "'"), toString(line));
		// add : expected '{'

	eraseToken(qu, line);
	throwIfFileIsEmpty(qu, line);

	while (qu.size() && qu.front().front() != "}")
		callDoers(qu, location, line);

	conf.set_location(uri, location);

	if (qu.empty() || qu.front().front() != "}")
		throwParsingError("expected '}'", toString(line));

	eraseToken(qu, line);
}
