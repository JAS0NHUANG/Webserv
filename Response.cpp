#include "Response.hpp"
#include "incs/Client.hpp"
#include "Cgi.hpp"

bool Response::send_informational_response() const {
	return true;
}

bool Response::send_successful_response() const {

	std::string response;
	// if (_code == 200)
	// 	response = "HTTP/1.1 200 OK\n\nHello world\n";
	// else if (_code == 201) {
	// 	response = "HTTP/1.1 201 Created\nLocation: /\n\n";
	// }
	response = "HTTP/1.1 200 OK\n\nHello world\n";
	if (send(this->client.get_fd(), response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

bool Response::send_redirection_message() const {
	return true;
}

bool Response::send_client_error_response() const {
	// NOTE : Search into Config if there is default
	// error pages set.
	std::string response;
	std::string myline;

	response = "HTTP/1.1 ";
	response += toString(this->status_code);
	response += " ";
	response += this->get_code_msg(this->status_code);
	response += "\n";
	response += "Server: Tengine\nConnection: keep-alive\nDate: Wed, 30 Nov 2016 07:58:21 GMT\nCache-Control: no-cache\nContent-Type: text/html;charset=UTF-8\nKeep-Alive: timeout=20\nVary: Accept-Encoding\nPragma: no-cache\nX-NWS-LOG-UUID: bd27210a-24e5-4740-8f6c-25dbafa9c395\nContent-Length: 180945\n\r\n";
	std::ifstream myfile ("tests/www/errors/404.html");
	if (myfile.is_open()){
		while(myfile){
			std::getline (myfile, myline);
			response += myline;
		}
	}
	std::cerr << "hteml :" <<response << "\n";
	// if (this->client.get_code() == 400)
	// 	response = "HTTP/1.1 400 Bad Request\n";
	// else if (this->client.get_code() == 404)
	// 	response = "HTTP/1.1 405 Method Not Allowed\n";
	// else if (this->client.get_code() == 405)
	// 	response = "HTTP/1.1 405 Method Not Allowed\n";

	if (send(this->client.get_fd(), response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

bool Response::send_server_error_response() const {

	std::string response;
	if (this->client.get_code() == 500)
		response = "HTTP/1.1 500 Internal Server Error\n";
	else if (this->client.get_code() == 501)
		response = "HTTP/1.1 501 Not Implemented\n";
	else if (this->client.get_code() == 505)
		response = "HTTP/1.1 505 HTTP Version Not Supported\n";

	if (send(this->client.get_fd(), response.c_str(), response.size(), 0) < 0)
			errMsgErrno("send failed");
	return true;
}

Response::Response(Client client): client(client){
	std::cerr << "response creet!!!\n";
	this->status_code_list = init_code_msg();
	this->http_version = "HTTP/1.1";
	this->status_code = 0;
}
Response::~Response(void){

}
bool Response::send_response(){
	// NOTE : TO DEV
	// Returns true if we need to close the connection
	// If send() fails return true anyway
	std::cout << "Sending response\n";
	std::cerr << "\n";
	this->status_code = this->client.get_code();

	bool close_conn = false;
	// if (_code >= 200 && _code <= 299)
	// 	close_conn = send_successful_response();
	if (this->status_code >= 400 && this->status_code <= 499)
		return send_client_error_response();
	else if (this->status_code >= 500 && this->status_code <= 599)
		return send_server_error_response();
	
	//redirection???
	//check_httpvserion();
	Config conf = this->client.get_conf();
	conf.debug();
	std::cerr << "std::map" << this->client.get_request_target() << "\n";
	std::cerr << "std::map<std::string, Location>" << conf.get_location(this->client.get_request_target()).first << "\n";
	
	//if location exist;
	Location check_location = conf.get_location(this->client.get_request_target()).second;
	if (conf.get_location(this->client.get_request_target()).first == false){
		std::cerr << "INDSIE\n";
		this->status_code = 404;
		return send_client_error_response();
	}else if(check_location.get_cgi().first == true) {
		//location is cgi
		std::cerr << "here is cgi\n";
		Cgi test_cgi(this->client, conf);
		std::string script = check_location.get_cgi().second.second;
		std::string body = test_cgi.handler(const_cast<char*>(script.c_str()));

		return true;
	}else {
		//if_method_allow?

	}
	//else if ()
	//conf.get_location(this->client.get_path()).second;
	std::cerr << "if cgi" <<  check_location.get_cgi().first << "\n";
	std::cerr << "if cgi" <<  check_location.get_cgi().first << "\n";
	//if (this->client->get_path()

	// }else if method  get{
	// 	if autoindex()

	// } else if method post{

	// } else if method deleted{

	// }


	if (this->client.get_code() == 0) { // Temporarys statement (for test purpose)
		std::string response_ex("HTTP/1.1 200 OK\n\nHello world\n");
		if (send(this->client.get_fd(), response_ex.c_str(), response_ex.size(), 0) < 0)
			errMsgErrno("send failed");
		return true;
	}

	return close_conn;
}
std::string Response::get_code_msg(int status_code) const{
    return this->status_code_list.find(status_code)->second;
}
std::map<int, std::string> Response::init_code_msg()
{
	std::map<int, std::string> status;
	
	status[200] = "OK";
	status[201] = "Created";
	status[202] = "Accepted";
	status[203] = "Non-Authoritative Information";
	status[204] = "No Content";
	status[205] = "Reset Content";
	status[206] = "Partial Content";
	status[207] = "Multi-Status";
	status[208] = "Already Reported";
	status[226] = "IM Used";
	status[300] = "Multiple Choices";
	status[301] = "Moved Permanently";
	status[302] = "Found";
	status[303] = "See Other";
	status[304] = "Not Modified";
	status[305] = "Use Proxy";
	status[306] = "Switch Proxy";
	status[307] = "Temporary Redirect";
	status[308] = "Permanent Redirect";
	status[400] = "Bad Request";
	status[401] = "Unauthorized";
	status[402] = "Payment Required";
	status[403] = "Forbidden";
	status[404] = "Not Found";
	status[405] = "Method Not Allowed";
	status[406] = "Not Acceptable";
	status[407] = "Proxy Authentication Required";
	status[408] = "Request Timeout";
	status[409] = "Conflict";
	status[410] = "Gone";
	status[411] = "Length Required";
	status[412] = "Precondition Failed";
	status[413] = "Payload Too Large";
	status[414] = "URI Too Long";
	status[415] = "Unsupported Media Type";
	status[416] = "Range Not Satisfiable";
	status[417] = "Expectation Failed";
	status[418] = "I\'m a teapot"; 
	status[421] = "Misdirected Request";
	status[422] = "Unprocessable Entity";
	status[423] = "Locked";
	status[424] = "Failed Dependency";
	status[425] = "Too Early";
	status[426] = "Upgrade Required";
	status[428] = "Precondition Required";
	status[429] = "Too Many Requests";
	status[431] = "Request Header Fields Too Large";
	status[451] = "Unavailable For Legal Reasons";
	status[500] = "Internal Server Error";
	status[501] = "Not Implemented";
	status[502] = "Bad Gateway";
	status[503] = "Service Unavailable";
	status[504] = "Gateway Timeout";
	status[505] = "HTTP Version Not Supported";
	status[506] = "Variant Also Negotiates";
	status[507] = "Insufficient Storage";
	status[508] = "Loop Detected";
	status[510] = "Not Extended";
	status[511] = "Network Authentication Required";
	return status;
}
