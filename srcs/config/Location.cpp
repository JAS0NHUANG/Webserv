#include "Location.hpp"

void debug_print(std::set<std::string> x) {
	for (std::set<std::string>::iterator it = x.begin(); it != x.end(); it++)
		std::cout << *it << " ";
	std::cout << "\n";
}

void debug_print(std::map<int, std::string> x) {
	for (std::map<int, std::string>::iterator it = x.begin(); it != x.end(); it++)
		std::cout << "[ " << it->first << " | " << it->second << " ] ";
	std::cout << "\n";
}

void debug_print(std::vector<std::string> x) {
	for (std::vector<std::string>::iterator it = x.begin(); it != x.end(); it++)
		std::cout << *it << " ";
	std::cout << "\n";
}
void debug_print(std::map<int, std::pair<int, std::string> > x) {
	std::map<int, std::pair<int, std::string> >::iterator it = x.begin();
	for (; it != x.end(); it++)
		std::cout << "cgi :[ " << it->first << " | " << it->second.first << " | " << it->second.second << "] ";
	std::cout << "\n";
}

void debug_print(std::pair<bool, std::pair<std::string, std::string> > x) {
	std::cout << "[ " << x.first << " | ( " << x.second.first << " | " << x.second.second << " ) ]\n";
}

void debug_print(std::pair<bool, std::string> x) {
	std::cout << "[ " << x.first << " | " << x.second << " ]\n";
}

// void debug_print(std::map<std::string, std::string> x) {
// 	for (std::map<std::string, std::string>::iterator it = x.begin(); it != x.end(); it++)
// 		std::cout << "[ " << it->first << " | " << it->second << " ]\n";
// }
// void debug_print(std::pair<bool, std::string> x) {
// 	std::cout << "[ " << x.first << " | " << x.second << " ]\n";
// }

void debug_print(std::map<std::string, std::string> x) {
	for (std::map<std::string, std::string>::iterator it = x.begin(); it != x.end(); it++)
		std::cout << "[ " << it->first << " | " << it->second << " ]\n";
}

void	Location::debug() const {
	std::cout << MAG ;
	std::cout << "allow_method : "; debug_print(_allow_method);
	std::cout << "return       : " << _return << "\n";
	std::cout << "return status: " << _return_status << "\n";
	std::cout << "root         : " << _root	<< "\n";
	std::cout << "autoindex    : " << _autoindex << "\n";
	std::cout << "index        : "; debug_print(_index);
	std::cout << "cgi          : "; debug_print(_cgi);
	std::cout << "upload_store : "; debug_print(_upload_store);
	std::cout << RESET ;
}

Location::Location() {
	_allow_method.insert("POST");
	_allow_method.insert("GET");
	_allow_method.insert("DELETE");

	_root = "/var/www/html";
	_autoindex = false;
	_index.push_back("index.html");
	//_cgi.first = false;
	_upload_store.first = false;
}

Location::Location(const Location &x) {
	*this = x;
}

Location::~Location() {}

Location& Location::operator=(const Location &x) {
	_allow_method	= x._allow_method;
	_return			= x._return;
	_return_status	= x._return_status;
	_root			= x._root;
	_autoindex		= x._autoindex;
	_index			= x._index;
	_cgi			= x._cgi;
	_upload_store	= x._upload_store;
	return *this;
}

bool Location::is_method_allowed(std::string method) const {
	if (_allow_method.find(method) == _allow_method.end())
		return false;
	return true;
}

std::string Location::get_return() const {
	return _return;
}

std::string Location::get_return_status() const {
	return _return_status;
}

std::string Location::get_root() const {
	return _root;
}

bool Location::get_autoindex() const {
	return _autoindex;
}

std::vector<std::string> Location::get_index() const {
	return _index;
}

std::pair<bool, std::string> Location::get_cgi(std::string ext) const {
	if (_cgi.count(ext) == 0)
		return std::pair<bool, std::string>(false, "");
	std::string bin = (_cgi.find(ext)->second);
	return std::pair<bool, std::string>(true, bin);
}
// std::pair<bool, std::pair<std::string, std::string> > Location::get_cgi() const {
// 	return _cgi;
// }
std::pair<bool, std::string> Location::get_upload_store() const {
	return _upload_store;
}

void Location::set_allow_method(std::vector<std::string> &vec) {
	_allow_method.clear();
	_allow_method.insert(vec.begin(), vec.end());
}

void	Location::set_return_status(std::string &str) {
	_return_status = str;
}

void	Location::set_return(std::string &str) {
	_return = str;
}


void	Location::set_root(std::string &str) {
	_root = str;
}

void	Location::set_autoindex(bool b) {
	_autoindex = b;
}

void	Location::set_index(std::vector<std::string> &vec) {
	_index.clear();
	_index.assign(vec.begin(), vec.end());
}

void	Location::set_cgi(std::string &ext, std::string &bin) {
	_cgi[ext] = bin;
}

// void	Location::set_cgi(std::string &ext, std::string &path) {
// 	_cgi.first = true;
// 	_cgi.second = std::make_pair(ext, path);
// }

void	Location::set_upload_store(std::string &dir) {

	_upload_store.first = true;
	_upload_store.second = dir;
}
