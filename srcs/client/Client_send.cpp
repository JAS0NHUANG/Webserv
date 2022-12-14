#include "Client.hpp"

bool Client::send_informational_response() const {
	return true;
}

bool Client::send_successful_response() const {

	std::string response;
	if (_status_code == 200)
		response = "HTTP/1.1 200 OK\n\nHello world\n\n";
	else if (_status_code == 201) {
		response = "HTTP/1.1 201 Created\nLocation: /\n\n";
	}

	if (send(_fd, response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

bool Client::send_redirection_message() const {
	return true;
}

bool Client::send_client_error_response() const {
	// NOTE : Search into Config if there is default
	// error pages set.
	std::string response;

	if (_status_code == 400)
		response = "HTTP/1.1 400 Bad Request\n\n";
	else if (_status_code == 404)
		response = "HTTP/1.1 404 Not Found\n\n";
	else if (_status_code == 405)
		response = "HTTP/1.1 405 Method Not Allowed\n\n";
	else if (_status_code == 408)
		response = "HTTP/1.1 408 Request Timeout\n\n";
	else if (_status_code == 413)
		response = "413 Payload Too Large\n\n";
	else if (_status_code == 414)
		response = "HTTP/1.1 414 URI Too Long\n\n";

	if (send(_fd, response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

bool Client::send_server_error_response() const {

	std::string response;
	if (_status_code == 500)
		response = "HTTP/1.1 500 Internal Server Error\n\n";
	else if (_status_code == 501)
		response = "HTTP/1.1 501 Not Implemented\n\n";
	else if (_status_code == 505)
		response = "HTTP/1.1 505 HTTP Version Not Supported\n\n";

	if (send(_fd, response.c_str(), response.size(), 0) < 0)
			errMsgErrno("send failed");
	return true;
}

bool Client::send_response() {
	// NOTE : TO DEV
	// Returns true if we need to close the connection
	// If send() fails return true anyway
	std::cout << "Sending response\n";

	if (_status_code == 0) { // Temporarys statement (for test purpose)
		std::string response("HTTP/1.1 200 OK\n\nHello world\n\n");
		if (send(_fd, response.c_str(), response.size(), 0) < 0)
			errMsgErrno("send failed");
		return true;
	}

	bool close_conn = false;
	if (_status_code >= 200 && _status_code <= 299)
		close_conn = send_successful_response();
	else if (_status_code >= 400 && _status_code <= 499)
		close_conn = send_client_error_response();
	else if (_status_code >= 500 && _status_code <= 599)
		close_conn = send_server_error_response();

	return close_conn;
}
