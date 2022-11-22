#include "Client.hpp"

void Client::remove_cr_char(std::deque<std::string> &lines) {
	std::deque<std::string>::iterator it;

	for (it = lines.begin(); it != lines.end(); it++) {
		if (!(*it).empty() && (*it)[(*it).size() - 1] == '\r') 
			(*it).erase((*it).size() - 1, 1);
	}
}

void Client::process_request_line(std::string &line) {
	std::cout << "process_request_line\n";

	std::vector<std::string> tokens = ft_split(line.c_str(), "\t\v\r ");
	std::vector<std::string>::iterator it = tokens.begin();
	for (; it != tokens.end() && (*it).empty(); it++) ;

	if (it == tokens.end())
		return;

	// Change method to a simple string
	if (*it == "GET") {
		if(!_conf.is_method_allowed(GET))
			throw 405;
		_method = "GET";
	}
	else if (*it == "POST") {
		if(!_conf.is_method_allowed(POST))
			throw 405;
		_method = "POST";
	}
	else if (*it == "DELETE") {
		if(!_conf.is_method_allowed(DELETE))
			throw 405;
		_method = "DELETE";
	}
	else {
		std::cerr << RED "501 NOT IMPLEMENTED\n" RESET;
		throw 501;
	}

	if (++it == tokens.end())
		throw 400;

	// NOTE : if request-target is in absolute form, we need to parse it

	// NOTE : TO DO 
	// if ((*it).size() > 16000) {
	// 	std::cerr << RED "501 Not Implemented URI Too Long";
	// 	throw 501;
	// }

	_path = *it;

	if (++it == tokens.end())
		throw 400;

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

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Host

// https://developer.mozilla.org/en-US/docs/Glossary/Forbidden_header_name
// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers

// NOTE : What to do if same header names are received ???
void Client::process_field_line(std::string &line) {
	std::cout << "process_headers\n";
	std::string					field_name;
	std::vector<std::string>	field_values;

	std::string::size_type i = line.find(":");
	if (i != std::string::npos) {
		field_name.assign(line.begin(), line.begin() + i);
		if (field_name.empty() || field_name_has_whitespace(field_name)) 
			throw 400;
		str_to_lower(field_name);
		line.erase(0, i + 1);
		// NOTE : TOKENIZE FIELD VALUE IN ACCORDANCE WITH RFC 9112
		_headers[field_name] = std::vector<std::string>(); // Fill with the list of field values
		return;
	}
	else if (line.empty()) {
		if (_headers.count("host") == 0) // No Host received
			throw 400;
		_process_headers = false; // All headers has been received
		return;
	}
	throw 400; // Malformed header
}

void Client::process_body(std::string &line) {
	std::cout << "process_body\n";
	// The presence of a message body in a request is signaled
	// by a Content-Length or Transfer-Encoding header field.

	// NOTE : Each time this function is called, need to check max_body_size

	if (_headers.count("transfer-encoding") == 0 &&
		_headers.count("content-length") == 0) // No body
		return ;

	if (line.empty()) {
		_code = 201;
		return;
	}

	if (_body.size() + line.size() > _conf.get_client_max_body_size()) {
		// NOTE : handle this error
		std::cerr << RED "ERR_CODE MAX BODY SIZE REACHED\n" RESET;
		return;
	}
	_body += line;
}

void Client::parse_line(std::deque<std::string> &lines) {
	std::deque<std::string>::iterator it = lines.begin();

	remove_cr_char(lines);

	for (; it != lines.end(); it++)
		std::cout << BCYN "LINE : |" << *it <<  "|\n" RESET;

	while (!lines.empty()) {
		if (_process_request_line)
			process_request_line(lines.front());
		else if (_process_headers) {
			process_field_line(lines.front());
			// Maybe parse the header right here to know what to do next
		}
		else
			process_body(lines.front());

		lines.pop_front();
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
		if (valread == 0)
			break;
		else if (valread < 0) {
			if (errno == EAGAIN) {
				// if all the headers has been read,
				// Host: has been received and the method is GET
				// then send the response

				// NOTE : Create a function that check if the request is complete
				if (_method == "GET" && _process_headers == false)
					break;
				return false;
			}
			errMsgErrno("recv");
			throw 500; // Internal server error
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
				break;
			}
		}
	}
	return true; // true if the request is finished
}