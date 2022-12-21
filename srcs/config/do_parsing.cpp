#include "webserv.hpp"

void do_server_parsing(std::queue<std::vector<std::string> > &qu, int &line) {
	if (qu.front().front() != "server")
		throw_parsing_error(std::string("unexpected '" + qu.front().front() + "'"), to_String(line));

	erase_token(qu, line);

	if (qu.front().front() != "{")
		throw_parsing_error("expected '{'", to_String(line));

	erase_token(qu, line);
}

void do_listen_parsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	std::string address, port;
	std::string str = qu.front().front();
	std::string::size_type i = 0;

	i = str.find(":", i);
	if (i == std::string::npos)
		throw_parsing_error("listen directive bad arguments", to_String(line));

	address = str.substr(0, i);
	port = str.substr(i + 1, str.size() - i);

	if (address == "localhost")
		conf.set_address("127.0.0.1");
	else if (address == "*")
		conf.set_address("0.0.0.0");
	else if (is_valid_ip_address(address))
		conf.set_address(address);
	else {
		throw_parsing_error("invalid ip address", to_String(line));
		return;
	}

	if (!is_num(port) || !is_valid_port(port)) {
		throw_parsing_error("invalid port number", to_String(line));
		return ;
	}
	conf.set_port(to_int(port));

	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throw_parsing_error("expected ';'", to_String(line));

	erase_token(qu, line);
}

void do_server_name_parsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	std::vector<std::string> names;
	while (qu.size() && qu.front().front() != ";") {
		if (qu.front().front() == "}" || qu.front().front() == "{")
			throw_parsing_error("expected ';'", to_String(line));
		names.push_back(qu.front().front());
		erase_token(qu, line);
		throw_if_file_is_empty(qu, line);
	}

	conf.set_server_name(names);

	if (qu.empty())
		throw_parsing_error("expected ';'", to_String(line));

	erase_token(qu, line);
}


void do_error_page_parsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	std::vector<std::string> strvec;
	while (qu.size() && qu.front().front() != ";") {
		strvec.push_back(qu.front().front());
		erase_token(qu, line);
		throw_if_file_is_empty(qu, line);
	}

	if (strvec.size() < 2)
		throw_parsing_error("error_page directive need a minimum of 2 arguments", to_String(line));

	std::string uri = strvec.back();
	strvec.pop_back();

	std::vector<int> codes;
	std::vector<std::string>::iterator it = strvec.begin();
	for (; it != strvec.end(); it++) {
		if (is_num(*it) == false)
			throw_parsing_error(std::string(*it + " is not an integer"), to_String(line));

		if (is_http_error_status_code(*it) == false)
			throw_parsing_error(std::string(*it + " is not a valid http status code"), to_String(line));

		codes.push_back(to_int(*it));
	}

	conf.set_error_page(codes, uri);

	if (qu.empty() || qu.front().front() != ";")
		throw_parsing_error("expected ';;", to_String(line));

	erase_token(qu, line);
}

void do_client_max_body_size_parsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	std::string::iterator it = qu.front().front().end() - 1;
	if (*it != 'm' && *it != 'M')
		throw_parsing_error("expected 'm' to precise memory capacity unit", to_String(line));

	qu.front().front().erase(it);


	if (is_num(qu.front().front()) == false)
		throw_parsing_error(std::string(qu.front().front() + " is not an integer"), to_String(line));

	conf.set_client_max_body_size(to_ULL(qu.front().front()) * 1000000);
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throw_parsing_error("expected ';'", to_String(line));

	erase_token(qu, line);
}

void do_allow_method_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	std::vector<std::string> vec;
	while (qu.size() && qu.front().front() != ";") {
		str_to_upper(qu.front().front());
		if (is_valid_method(qu.front().front()) == false)
			throw_parsing_error(std::string(qu.front().front() + " is not a valid method"), to_String(line));

		vec.push_back(qu.front().front());
		erase_token(qu, line);
		throw_if_file_is_empty(qu, line);
	}

	conf.set_allow_method(vec);

	if (qu.empty() || qu.front().front() != ";")
		throw_parsing_error("expected ';'", to_String(line));

	erase_token(qu, line);
}

void do_return_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if (qu.front().front().find_first_not_of("0123456789") != std::string::npos || 
		(to_int(qu.front().front()) < 300 || to_int(qu.front().front()) > 399))
		throw_parsing_error("expected a redirection status code `3xx`", to_String(line));

	conf.set_return_status(qu.front().front());
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if (qu.front().front() == ";")
		throw_parsing_error("unexpected ';'", to_String(line));

	conf.set_return(qu.front().front());
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throw_parsing_error("expected ';'", to_String(line));

	erase_token(qu, line);
}

void do_root_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if ((qu.front().front().size() > 1) && *(qu.front().front().end() - 1) == '/')
		qu.front().front().erase(qu.front().front().size() - 1);

	conf.set_root(qu.front().front());
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throw_parsing_error("expected ';'", to_String(line));

	erase_token(qu, line);
}

void do_autoindex_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	str_to_upper(qu.front().front());
	if (qu.front().front() != "ON" && qu.front().front() != "OFF")
		throw_parsing_error("expected 'on' of 'off'", to_String(line));

	if (qu.front().front() == "ON")
		conf.set_autoindex(true);
	else
		conf.set_autoindex(false);

	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throw_parsing_error("expected ';'", to_String(line));

	erase_token(qu, line);
}

void do_index_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	conf.set_index(qu.front().front());
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throw_parsing_error("expected ';'", to_String(line));

	erase_token(qu, line);
}

void do_cgi_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	std::string ext = qu.front().front();

	if (ext[0] != '.')
		ext = "." + ext;

	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	std::string path = qu.front().front();
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	conf.set_cgi(ext, path);

	if (qu.empty() || qu.front().front() != ";")
		throw_parsing_error("expected ';'", to_String(line));

	erase_token(qu, line);
}

void do_upload_store_parsing(std::queue<std::vector<std::string> > &qu, Location &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	conf.set_upload_store(qu.front().front());

	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if (qu.empty() || qu.front().front() != ";")
		throw_parsing_error("expected ';'", to_String(line));

	erase_token(qu, line);
}

void do_location_parsing(std::queue<std::vector<std::string> > &qu, Config &conf, int &line) {
	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	std::string uri = qu.front().front();

	if ((uri.size() > 1) && *(uri.end() - 1) == '/')
		uri.erase(uri.size() - 1);

	Location location(conf);

	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	if (qu.front().front() != "{")
		throw_parsing_error(std::string("unexpected '" + qu.front().front() + "'"), to_String(line));
		// add : expected '{'

	erase_token(qu, line);
	throw_if_file_is_empty(qu, line);

	while (qu.size() && qu.front().front() != "}")
		call_doers(qu, location, line);

	conf.set_location(uri, location);

	if (qu.empty() || qu.front().front() != "}")
		throw_parsing_error("expected '}'", to_String(line));

	erase_token(qu, line);
}
