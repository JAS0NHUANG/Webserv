#include "Client.hpp"

void Client::remove_cr_char(std::deque<std::string> &lines) {
	std::deque<std::string>::iterator it;

	for (it = lines.begin(); it != lines.end(); it++) {
		if (!(*it).empty() && (*it)[(*it).size() - 1] == '\r') 
			(*it).erase((*it).size() - 1, 1);
	}
}

std::string Client::get_query_string(std::string &request_target) {
	std::string str;

	std::string::size_type i = request_target.find("?");
	if (i != std::string::npos) {
		str.assign(request_target.begin() + i, request_target.end());
		request_target.erase(i, request_target.size());
	}

	return str;
}

std::string Client::get_path(std::string request_target) {
	std::string real_path;

	if (request_target[0] != '/')
		request_target = "/" + request_target;

	std::string::iterator it = request_target.begin() + 1;
	for (; it != request_target.end(); it++) {
		if (*it == '/')
			break;
	}

	std::string location(request_target.begin(), it);

	request_target.erase(request_target.begin(), it);

	std::pair<bool, Location> pr = _conf.get_location(location);
	if (pr.first)
		location = pr.second.get_root() + location + request_target;
	else
		location = _conf.get_root() + location + request_target;

	return location;
}

void Client::check_method(std::string &method) {
	if(!_conf.is_method_allowed(method))
		throw 405;
}

void Client::check_access(std::string request_target) {

	_path = get_path(request_target);

	int code = access(_path.c_str(), 0);
	std::cerr << "access" << _path.c_str() << "\n";
	if (code < 0) {
		errMsgErrno("access");
		if (errno == ENOENT)
			throw 404; // Not Found
		throw 500; //Internal error server
	}
}

void Client::process_request_line(std::string &line) {

	std::vector<std::string> tokens = ft_split(line.c_str(), "\t\v\r ");
	std::vector<std::string>::iterator it = tokens.begin();

	// Check if method is supported by webserv
	if (*it != "GET" && *it != "POST" && *it != "DELETE")
		throw 501;
	_method = *it;

	// Check if request target has been recv
	if (++it == tokens.end())
		throw 400;

	// Check is uri is too long
	if ((*it).size() > 418000)
		throw 414;

	// Get request target and query string if any
	// and delete it from request_target
	_request_target = *it;
	_query_string =  get_query_string(_request_target);

	// Check if there is the http version
	if (++it == tokens.end())
		throw 400;

	// Check if the http version is valid
	if (*it != "HTTP/1.1")
		throw 400;

	_process_request_line = false;
}

bool Client::field_name_has_whitespace(std::string &field_name) const {
	if (field_name[field_name.size() - 1] == ' '  || 
		field_name[field_name.size() - 1] == '\t' ||
		field_name[field_name.size() - 1] == '\r' ||
		field_name[field_name.size() - 1] == '\f' ||
		field_name[field_name.size() - 1] == '\v')
		return true;
	return false;
}
void Client::retrieve_conf(std::string host) {

	std::string::size_type i = host.find(":");
	if (i != std::string::npos)
		host.erase(i);
	for (i = 0; i < host.size() && isWhitespace(host[i]); i++);

	if (i != 0)
		host.assign(host.begin() + i, host.end());

	_conf = *(_virtual_servers.begin());
	std::vector<Config>::iterator it = _virtual_servers.begin();
	for (; it != _virtual_servers.end(); it++) {
		std::vector<std::string> serv_names = (*it).get_server_name(); 
		for (std::vector<std::string>::size_type i = 0; i != serv_names.size(); i++) {
			if (serv_names[i] == host) {
				_conf = *it;
				break;
			}
		}
	}
}
void Client::process_field_line(std::string &line) {
	std::string					field_name;
	std::vector<std::string>	field_values;

	std::string::size_type i = line.find(":");
	if (i != std::string::npos) {
		field_name.assign(line.begin(), line.begin() + i);
		if (field_name.empty() || field_name_has_whitespace(field_name)) 
			throw 400;
		str_to_lower(field_name);
		line.erase(0, i + 1);
		_headers[field_name] = line;
		return;
	}
	else if (line.empty()) {
		if (_headers.count("host") == 0) // No Host received
			throw 400;
		_process_headers = false; // All headers has been received
		if (_method != "POST")
			_request_is_complete = true;
		// Search for the configuration whose server_names match with host 
		// by default the first server for a host:port will be
		// the default for this host:port
		retrieve_conf(_headers["host"]);

		// Check if the method is allowed by the virtual server
		check_method(_method);

		// Check is the resource is accessible
		check_access(_request_target);
		return;
	}
	throw 400; // Malformed header
}

void Client::process_body(std::string &line) {
	// The presence of a message body in a request is signaled
	// by a Content-Length or Transfer-Encoding header field.

	// NOTE : Each time this function is called, need to check max_body_size

	// We dont expect receiving a body if the method is not POST
	if (_method != "POST") {
		_request_is_complete = true; 
		return;
	}

	if (_headers.count("transfer-encoding") == 0 &&
		_headers.count("content-length") == 0) { // No body
		_request_is_complete = true;
		return ;
	}

	if (line.empty()) {
		_request_is_complete = true;
		return;
	}

	if (_body.size() + line.size() > _conf.get_client_max_body_size())
		throw 413;

	_body += line;
}

void Client::parse_line(std::deque<std::string> &lines) {

	remove_cr_char(lines);

	while (!lines.empty()) {
		if (_process_request_line)
			process_request_line(lines.front());
		else if (_process_headers)
			process_field_line(lines.front());
		else
			process_body(lines.front());

		lines.pop_front();
		if (_request_is_complete)
			break;
	}
}

std::deque<std::string> Client::getlines(std::string buf) {
	std::deque<std::string> lines;
	_ss << buf;

	std::string::size_type i = buf.find("\n");
	while (i != std::string::npos) {
		std::string inter;
		std::getline(_ss, inter);
		lines.push_back(inter);
		buf.erase(0, i + 1);
		i = buf.find("\n");
	}
	return lines;
}

bool Client::recv_request() {
	std::deque<std::string>	lines;
	char buffer[BUFFER_SIZE] = {0};

	while (true) {
		int valread = recv(_fd, buffer, BUFFER_SIZE - 1, 0);
		if (valread == 0) {
			if (_process_request_line == true || _process_headers == true)
				_request_is_complete = true;
			_code = 400;
			return true;
		}
		else if (valread < 0) {
			errMsgErrno("recv");
			break;
		}
		buffer[valread] = '\0';
		lines = getlines(buffer);
		if (!lines.empty()) {
			try { // This will catch any bad request 
				parse_line(lines);
			}
			catch (int error_code) {
				std::cout << "Catched exception: " << error_code << "\n" ;
				_code = error_code;
				return true;
			}
		}
		if (_request_is_complete)
			break;
	}
	if (!_request_is_complete) {
		return false;
	}
	return true; // true if the request is finished
}