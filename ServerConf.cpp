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
	_address				= "*";
	_port					= 80;
	_client_max_body_size	= 1000000; // 1M
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
	_address				= x._address;
	_port					= x._port;
	_server_name			= x._server_name;
	_error_page				= x._error_page;
	_client_max_body_size 	= x._client_max_body_size;
	_location				= x._location;
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
	return _server_name;
}

std::string ServerConf::get_error_page(int code) const {
	if (_error_page.count(code))
		return _error_page.find(code)->second;
	return std::string();
}

unsigned long long ServerConf::get_client_max_body_size() const {
	return _client_max_body_size;
}

std::pair<bool, BaseConf>	ServerConf::get_location(std::string uri) const {
	if (_location.count(uri))
		return std::make_pair(true, _location.find(uri)->second);
	return std::make_pair(false, BaseConf());
}

char const* ServerConf::NotFoundException::what() const throw() {
	return "Not found";
}

void	ServerConf::set_address(std::string str) {
	_address = str;
}

void	ServerConf::set_port(int port) {
	_port = port;
}

void	ServerConf::set_server_name(std::vector<std::string> names) {
	std::vector<std::string>::iterator it = names.begin();

	for(; it != names.end(); it++)
		_server_name.push_back(*it);
}

void	ServerConf::set_error_page(std::vector<int> codes, std::string path) {
	std::vector<int>::iterator it = codes.begin();

	for (; it != codes.end(); it++) {
		_error_page.erase(*it);
		_error_page.insert(std::make_pair(*it, path));
	}
}

void	ServerConf::set_client_max_body_size(unsigned long long size) {
	_client_max_body_size = size;
}

void	ServerConf::set_location(std::string uri, BaseConf location) {
	_location.erase(uri);
	_location.insert(std::make_pair(uri, location));
}
