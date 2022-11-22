#include "Config.hpp"

void debug_print(std::map<std::string, Location> x) {
	for (std::map<std::string, Location>::iterator it = x.begin(); it != x.end(); it++) {
		if (it == x.begin())
			std::cout << "\n";
		std::cout << "- " << it->first << "\n";
		it->second.debug();
	}
	std::cout << "\n";
}

void Config::debug() const {
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

Config::Config() {
	_address				= "127.0.0.1";
	_port					= 8000;
	_client_max_body_size	= 1000000; // 1M
}

Config::~Config() {}

Config::Config(const Config &x) : Location() {
	*this = x;
}

Config& Config::Config::operator=(const Config &x) {
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

std::string Config::get_address() const {
	return _address;
}

int Config::get_port() const {
	return _port;
}

std::vector<std::string> Config::get_server_name() const {
	if (_server_name.size())
		return _server_name;
	return _server_name;
}

std::string Config::get_error_page(int code) const {
	if (_error_page.count(code))
		return _error_page.find(code)->second;
	return std::string();
}

unsigned long long Config::get_client_max_body_size() const {
	return _client_max_body_size;
}

std::pair<bool, Location>	Config::get_location(std::string uri) const {
	if (_location.count(uri))
		return std::make_pair(true, _location.find(uri)->second);
	return std::make_pair(false, Location());
}

char const* Config::NotFoundException::what() const throw() {
	return "Not found";
}

void	Config::set_address(std::string str) {
	_address = str;
}

void	Config::set_port(int port) {
	_port = port;
}

void	Config::set_server_name(std::vector<std::string> &names) {
	std::vector<std::string>::iterator it = names.begin();

	for(; it != names.end(); it++)
		_server_name.push_back(*it);
}

void	Config::set_error_page(std::vector<int> &codes, std::string &uri) {
	std::vector<int>::iterator it = codes.begin();

	for (; it != codes.end(); it++) {
		_error_page.erase(*it);
		_error_page.insert(std::make_pair(*it, uri));
	}
}

void	Config::set_client_max_body_size(unsigned long long size) {
	_client_max_body_size = size;
}

void	Config::set_location(std::string &uri, Location &location) {
	_location.erase(uri);
	_location.insert(std::make_pair(uri, location));
}
