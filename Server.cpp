#include "Server.hpp"

void debug_print(std::map<std::string, Location> x) {
	for (std::map<std::string, Location>::iterator it = x.begin(); it != x.end(); it++) {
		if (it == x.begin())
			std::cout << "\n";
		std::cout << "- " << it->first << "\n";
		it->second.debug();
	}
	std::cout << "\n";
}

void Server::debug() const {
	Location::debug();
	std::cout << MAG ;
	std::cout << "address      : " << _address << "\n";
	std::cout << "port         : " << _port << "\n";
	std::cout << "server_name  : "; debug_print(_server_name);
	std::cout << "error_page   : "; debug_print(_error_page);
	std::cout << "max_body_size: " << _client_max_body_size << "\n";
	std::cout << "location     : "; debug_print(_location);
	std::cout << RESET ;
}

Server::Server() {
	_address				= "*";
	_port					= 80;
	_client_max_body_size	= 1000000; // 1M
}

Server::~Server() {}

Server::Server(const Server &x) : Location() {
	*this = x;
}

Server& Server::Server::operator=(const Server &x) {
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

std::string Server::get_address() const {
	return _address;
}

int Server::get_port() const {
	return _port;
}

std::vector<std::string> Server::get_server_name() const {
	if (_server_name.size())
		return _server_name;
	return _server_name;
}

std::string Server::get_error_page(int code) const {
	if (_error_page.count(code))
		return _error_page.find(code)->second;
	return std::string();
}

unsigned long long Server::get_client_max_body_size() const {
	return _client_max_body_size;
}

std::pair<bool, Location>	Server::get_location(std::string uri) const {
	if (_location.count(uri))
		return std::make_pair(true, _location.find(uri)->second);
	return std::make_pair(false, Location());
}

char const* Server::NotFoundException::what() const throw() {
	return "Not found";
}

void	Server::set_address(std::string &str) {
	_address = str;
}

void	Server::set_port(int port) {
	_port = port;
}

void	Server::set_server_name(std::vector<std::string> &names) {
	std::vector<std::string>::iterator it = names.begin();

	for(; it != names.end(); it++)
		_server_name.push_back(*it);
}

void	Server::set_error_page(std::vector<int> &codes, std::string &uri) {
	std::vector<int>::iterator it = codes.begin();

	for (; it != codes.end(); it++) {
		_error_page.erase(*it);
		_error_page.insert(std::make_pair(*it, uri));
	}
}

void	Server::set_client_max_body_size(unsigned long long size) {
	_client_max_body_size = size;
}

void	Server::set_location(std::string &uri, Location &location) {
	_location.erase(uri);
	_location.insert(std::make_pair(uri, location));
}
