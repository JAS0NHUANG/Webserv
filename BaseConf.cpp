#include "BaseConf.hpp"

void debug_print(std::set<int> x) {
	for (std::set<int>::iterator it = x.begin(); it != x.end(); it++)
		std::cout << *it << " ";
	std::cout << "\n";
}

void debug_print(std::map<int, std::string> x) {
	for (std::map<int, std::string>::iterator it = x.begin(); it != x.end(); it++)
		std::cout << it->second << " ";
	std::cout << "\n";
}

void debug_print(std::vector<std::string> x) {
	for (std::vector<std::string>::iterator it = x.begin(); it != x.end(); it++)
		std::cout << *it << " ";
	std::cout << "\n";
}

void	BaseConf::debug() const {
	std::cout << MAG ;
	std::cout << "Debugging BaseConf\n";
	std::cout << "allow_method : "; debug_print(_allow_method);
	std::cout << "return       : "; debug_print(_return);
	std::cout << "root         : " << _root	<< "\n";
	std::cout << "autoindex    : " << _autoindex << "\n";
	std::cout << "index        : "; debug_print(_index);
	std::cout << "cgi          : "; debug_print(_cgi);
	std::cout << "upload_store : " << _upload_store << "\n"; 
	std::cout << RESET ;
} 


BaseConf::BaseConf() {
	_allow_method.insert(POST);
	_allow_method.insert(GET);
	_allow_method.insert(DELETE);

	// NOTE: _return : create a function that will set the default responses
	_root = "/var/www/html";
	_autoindex = false;
	_index.push_back("index");
	_index.push_back("index.html");
	_cgi.push_back("");
	_upload_store = false;
}

BaseConf::BaseConf(const BaseConf &x) {
	*this = x;
}

BaseConf::~BaseConf() {}

BaseConf& BaseConf::operator=(const BaseConf &x) {
	_allow_method	= x._allow_method;
	_return			= x._return;
	_root			= x._root;
	_autoindex		= x._autoindex;
	_index			= x._index;
	_cgi			= x._cgi;
	_upload_store	= x._upload_store;
	return *this;
}

std::vector<std::string> BaseConf::get_index() const {
	return _index;
}

bool BaseConf::is_method_allowed(int method) const { 
	if (_allow_method.count(method))
		return true;
	return false;
}

bool BaseConf::is_autoindex_on() const {
	return _autoindex;
}

bool BaseConf::is_cgi_set() const {
	if (_cgi[0] == "IS_SET")
		return true;
	return false;
}

bool BaseConf::is_upload_on() const {
	if (_upload_store == true)
		return true;
	return false;
}

bool BaseConf::is_return_set() const {
	if (_return.size())
		return true;
	return false;
}