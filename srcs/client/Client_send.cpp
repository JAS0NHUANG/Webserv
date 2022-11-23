#include "Client.hpp"
#include "../../Response.hpp"

bool Client::send_informational_response() const {
	return true;
}

bool Client::send_successful_response() const {

	std::string response;
	if (_code == 200)
		response = "HTTP/1.1 200 OK\n\nHello world\n";
	else if (_code == 201) {
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

	if (_code == 400)
		response = "HTTP/1.1 400 Bad Request\n";
	else if (_code == 405)
		response = "HTTP/1.1 405 Method Not Allowed\n";

	if (send(_fd, response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

bool Client::send_server_error_response() const {

	std::string response;
	if (_code == 500)
		response = "HTTP/1.1 500 Internal Server Error\n";
	else if (_code == 501)
		response = "HTTP/1.1 501 Not Implemented\n";
	else if (_code == 505)
		response = "HTTP/1.1 505 HTTP Version Not Supported\n";

	if (send(_fd, response.c_str(), response.size(), 0) < 0)
			errMsgErrno("send failed");
	return true;
}

bool Client::send_response() {
// NOTE : TO DEV
	// Returns true if we need to close the connection
	// If send() fails return true anyway
	std::cout << "Sending response\n";

	bool close_conn = 1;
	// if (_code >= 200 && _code <= 299)
	// 	close_conn = send_successful_response();
	if (_code >= 400 && _code <= 499)
		return send_client_error_response();
	else if (_code >= 500 && _code <= 599)
		return send_server_error_response();
	
	std::cerr << "_method:" << this->_method << "\n";
	std::cerr << "_method:" << this->_path << "\n";
	std::cerr << "_method:" << this->_body << "\n";

	close_conn = 0;
	//Response rsps(this);

	//redirection???
	//check_httpvserion();
	// check_location(){
	// 	if_location_exit?
	// 	//if_method_allow?
	// 	if_location_
	// }
	// if cgi{

	// }else if method  get{
	// 	if autoindex()

	// } else if method post{

	// } else if method deleted{

	// }


	if (_code == 0) { // Temporarys statement (for test purpose)
		std::string response("HTTP/1.1 200 OK\n\nHello world\n");
		if (send(_fd, response.c_str(), response.size(), 0) < 0)
			errMsgErrno("send failed");
		return true;
	}

	return close_conn;
}
