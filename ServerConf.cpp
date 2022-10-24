#include "ServerConf.hpp"

void debug_print(std::map<std::string, BaseConf> x) {
	for (std::map<std::string, BaseConf>::iterator it = x.begin(); it != x.end(); it++) {
		if (it == x.begin())
			std::cout << "\n";
		std::cout << "- " << it->first << "\n";
		it->second.debug();
	}
	std::cout << "\n";
}

void ServerConf::debug() const {
	BaseConf::debug();
	std::cout << MAG ;
	std::cout << "address      : " << _address << "\n";
	std::cout << "port         : " << _port << "\n";
	std::cout << "server_name  : "; debug_print(_server_name);
	std::cout << "error_page   : "; debug_print(_error_page);
	std::cout << "max_body_size: " << _client_max_body_size << "\n";
	std::cout << "location     : "; debug_print(_location);
	std::cout << RESET ;
}

ServerConf::ServerConf() {
	_address				= ""; // NOTE: What is the default ?
	_port					= 80;
	_client_max_body_size	= 1000000;
}

ServerConf::~ServerConf() {}

ServerConf::ServerConf(const ServerConf &x) : BaseConf() {
	*this = x;
}

ServerConf& ServerConf::ServerConf::operator=(const ServerConf &x) {
	_allow_method			= x._allow_method;
	_return					= x._return;
	_root					= x._root;
	_autoindex				= x._autoindex;
	_index					= x._index;
	_cgi					= x._cgi;
	_upload_store			= x._upload_store;
	_server_name			= x._server_name;
	_error_page				= x._error_page;
	_location				= x._location;
	_client_max_body_size 	= x._client_max_body_size;
	return *this;
}

std::string ServerConf::get_address() const {
	return _address;
}

int ServerConf::get_port() const {
	return _port;
}

std::vector<std::string> ServerConf::get_server_name() const {
	if (_server_name.size())
		return _server_name;
	throw NotFoundException();
}

std::string ServerConf::get_error_page(int code) const {
	if (_error_page.count(code))
		return _error_page.find(code)->second;
	throw NotFoundException();
}

unsigned long long ServerConf::get_client_max_body_size() const {
	return _client_max_body_size;
}

char const* ServerConf::NotFoundException::what() const throw() {
	return "Not found";
}
