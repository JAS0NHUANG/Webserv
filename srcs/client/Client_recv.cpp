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
		str.assign(request_target.begin() + i + 1, request_target.end());
		request_target.erase(i, request_target.size());
	}

	return str;
}

std::string Client::create_path(std::string request_target) {
	if (request_target[0] != '/')
		request_target = "/" + request_target;

	std::pair<bool, Location> pr;
	pr = _conf.get_location("/");
	std::pair<bool , Location> pr_candidate;
	pr_candidate.first = false;

	std::string location;
	std::string::iterator it = request_target.begin() + 1;
	while (it != request_target.end()){
		for (; it != request_target.end(); it++) {
			if (*it == '/')
				break;
		}
		location.assign(request_target.begin(), it);
		pr_candidate = _conf.get_location(location);
		if (pr_candidate.first == true)
			pr = pr_candidate;
		if (it != request_target.end())
			++it;
	}
	if (pr.first)
		location = pr.second.get_root() + location;
	else
		location = _conf.get_root() + location;
	return location;
}

void Client::check_method(std::string &method) {
	if(!_conf.is_method_allowed(method))
		throw 405;
}

void Client::check_access(std::string request_target) {

	_path = create_path(request_target);

	int code = access(_path.c_str(), 0);
	if (code < 0) {
		_syscall_error = "access() \"" + _path + "\"" + "failed (" + strerror(errno) + ")";
		log(log_error(_syscall_error), false);
		if (errno == ENOENT)
			throw 404; // Not Found
		throw 500; //Internal error server
	}
}

void Client::process_request_line(std::string &line) {

	_request_line = line;
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
	for (i = 0; i < host.size() && is_whitespace(host[i]); i++);

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

		if (field_name == "content-type") {
			std::string::size_type boundary_index = line.find("boundary");
			if (boundary_index != std::string::npos) {
				_body_boundary = line.erase(0, boundary_index + 9);
			}
		}
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

void Client::parse_line(std::deque<std::string> &lines, std::string &raw_request) {

	remove_cr_char(lines);

	while (!lines.empty()) {
		if (_process_request_line){
			process_request_line(lines.front());
			lines.pop_front();
		}
		else if (_process_headers)
		{
			process_field_line(lines.front());
			lines.pop_front();
		}
		else {
			process_body(raw_request);
			lines.clear();
		}

		if (_request_is_complete)
			break;
	}
	return ;
}

std::deque<std::string> Client::getlines(std::string buf) {
	std::deque<std::string> lines;
	_ss << buf;

	std::string::size_type i = buf.find("\n");
	while (!buf.empty()) {
		std::string inter;
		std::getline(_ss, inter);
		lines.push_back(inter);
		buf.erase(0, i + 1);
		i = buf.find("\n");
		if (i == std::string::npos) {
			i = buf.size();
		}
	}
	return lines;
}

bool Client::handle_request(std::string &raw_request) {
	// set status code too 400 when no raw_request recieved
	if (raw_request.empty()) {
		_request_is_complete = true;
		_status_code = 400;
	}
	std::deque<std::string>	lines;
	lines = getlines(raw_request);
	if (!lines.empty()) {
		try { // This will catch any bad request 
			parse_line(lines, raw_request);
		}
		catch (int error_code) {
			_status_code = error_code;
			return true;
		}
	}

	return _request_is_complete;
}

std::string Client::recv_request() {
	char buffer[BUFFER_SIZE] = {};
	int valread;
	std::string raw_request;

	while ((valread = recv(_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
		buffer[valread] = '\0';
		raw_request.append(buffer, valread);
	}
	if (valread == 0) {
		// change the condition from || to &&, dont know if it's logic?
		if (_process_request_line == false && _process_headers == false)
			_request_is_complete = true;
	}
	if (valread < 0) {
		_syscall_error = "recv()";
		log(log_error(_syscall_error), false);
		// Still don't know why sometimes will have "Resource temporarily unavailable" error...
		// Is something blocking the recv?
	}

	return raw_request;
}
