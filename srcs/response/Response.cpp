#include "Response.hpp"

Response::Response(Client client, std::map<std::string, std::map<std::string, std::string> > &sessions) :
	_client(client),
	_sessions(sessions)
{
	_path = _client.get_path();
	_conf = _client.get_conf();
	std::string tmp = _client.get_request_target();
	std::size_t found = tmp.find(".");
	while (found != std::string::npos)
	{
		tmp.erase(0, found + 1);
		_extension = "." + tmp;
		found = tmp.find(".");
	}
	init_code_msg();
	_status_code = _client.get_status_code();
	_if_location = _client.get_if_location();
	if (_if_location)
		_location = _conf.get_location(_client.get_request_target()).second;
}

Response::~Response(void)
{
}

std::string Response::set_session_cookie(const std::string &session_id,
										 const std::string &name,
										 const std::string &value) {
	_sessions[session_id][name] = value;
	std::string cookie_str = name + "=" + value;

	return cookie_str;
}

std::map<std::string, std::string> Response::parse_cookie(std::string& cookie_str) {
	std::map<std::string, std::string> cookies;
	std::size_t start = 0;

	remove(cookie_str, ' ');
	while (start < cookie_str.size()) {
		// Find the name of the next cookie
		std::size_t name_end = cookie_str.find('=', start);
		if (name_end == std::string::npos)
			break;
		std::string name = cookie_str.substr(start, name_end - start);
		// Find the value of the cookie
		std::size_t value_start = name_end + 1;
		std::size_t value_end = cookie_str.find(';', value_start);
		if (value_end == std::string::npos)
			value_end = cookie_str.size();
		std::string value = cookie_str.substr(value_start, value_end - value_start);
		// Store the cookie in the map
		cookies[name] = value;
		// Move to the next cookie
		start = value_end + 1;
  }
  return cookies;
}

void Response::check_all_cookies_received(std::map<std::string, std::string> &expected_cookies,
								std::map<std::string, std::string> &received_cookies,
								std::vector<std::string> &cookies) {
  // Iterate over the expected cookies
	std::map<std::string, std::string>::iterator it = expected_cookies.begin();
	for (; it != expected_cookies.end(); ++it) {
		// Check if the received cookies map contains the current cookie
		if (received_cookies.count(it->first) == 0 || received_cookies.find(it->first)->second != it->second) {
			// The current cookie is not present in the received cookies map
			cookies.push_back("Set-Cookie: " + set_session_cookie(expected_cookies["sessionId"], \
				it->first, it->second) + "\r\n");
		}
		// Check if the value of the current cookie in the received cookies map matches the expected value
			// The value of the current cookie does not match the expected value
	}

	// Iterate over the reveived cookies to check if there is unexpected ones
	it = received_cookies.begin();
	for (; it != received_cookies.end(); ++it) {
		if (expected_cookies.count(it->first) == 0) {
			cookies.push_back("Set-Cookie: " + it->first + "=deleted; Expires=Thu, 01 Jan 1970 00:00:00 GMT; Path=" + _client.get_request_target() + "\r\n");
		}
	}
}

std::string Response::create_random_session_id() {
  std::srand(std::time(NULL));
  static const char hex_digits[] = "0123456789abcdef";

  std::string session_id;
  for (int i = 0; i < 32; ++i) {
    session_id += hex_digits[std::rand() % 16];
  }
  return session_id;
}

void Response::manage_cookies(std::vector<std::string> &cookies) {
	if (_client.get_conf().get_cookies().empty() == false) {

		std::map<std::string, std::string> client_cookies = parse_cookie(_client.get_headers()["cookie"]);
		if (client_cookies.count("sessionId") == 0 || _sessions.count(client_cookies["sessionId"]) == 0) {
			std::string session_id = create_random_session_id();
			cookies.push_back("Set-Cookie: " + set_session_cookie(session_id, "sessionId", session_id) + "\r\n");
			std::vector<std::string> cookie_strings = _client.get_conf().get_cookies();
			std::vector<std::string>::iterator it = cookie_strings.begin();
			for (; it != cookie_strings.end(); it++) {
				std::string::iterator pos = std::find((*it).begin(), (*it).end(), '=');
				std::pair<std::string, std::string> pr = std::make_pair(std::string((*it).begin(), pos), std::string(pos + 1, (*it).end()));
			cookies.push_back("Set-Cookie: " + set_session_cookie(session_id, pr.first, pr.second) + "\r\n");
			}
		}
		else
			check_all_cookies_received(_sessions[client_cookies["sessionId"]], client_cookies, cookies);
		// else not setting any cookies
	}
}

void Response::set_header_fields(int cont_Leng)
{
	std::map<std::string, std::string>	headers;
	std::vector<std::string>			cookies;

	headers["Content-Length"] = to_String(cont_Leng);
	if (_status_code == 405)
	{
		std::string tmp;
		if ((_if_location && _location.is_method_allowed("GET") == true) ||
			(_conf.is_method_allowed("GET") == true))
			tmp = "GET, ";
		if ((_if_location && _conf.is_method_allowed("POST") == true) ||
			(_location.is_method_allowed("POST") == true))
			tmp += "POST, ";
		if ((_if_location && _conf.is_method_allowed("DELETE") == true) ||
			(_location.is_method_allowed("DELETE") == true))
			tmp += "DELETE, ";
		tmp = tmp.substr(0, tmp.size() - 2);
		headers["Allow"] = tmp;
	}

	if ((_if_location && _location.get_cgi(_extension).first == false) ||
		(_if_location == false &&_conf.get_cgi(_extension).first == false))
		headers["Content-Type"] = content_mime_type(this->_extension);
	else
		headers["Content-Type"] = "text/html";

	manage_cookies(cookies);

	if (_status_code >= 300 && _status_code < 400 && _if_location == true)
	{
		if (!_location.get_return().empty())
			headers["Location"] = _location.get_return();
		else
			headers["Location"] = _location.get_index();
	}
	else if (_status_code >= 300 && _status_code < 400 && _if_location == false)
	{
		if (!_conf.get_return().empty())
			headers["Location"] = _conf.get_return();
		else
			headers["Location"] = _conf.get_index();
	}
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	{
		_header_fields += it->first;
		_header_fields += ": ";
		_header_fields += it->second;
		_header_fields += "\r\n";
	}
	for (std::vector<std::string>::iterator it = cookies.begin(); it != cookies.end(); it++)
		_header_fields += *it;
}

bool Response::post_body()
{
	std::string response;

	_status_code = 201;
	_body = "<!DOCTYPE html><html><body><p>File/data successfully saved</p><p>";
	if (_client.get_headers()["content-type"].find("multipart") == std::string::npos)
	{
		_body += _client.get_body();
	}
	_body += "</p></body></html>";
	response = "HTTP/1.1 ";
	response += to_String(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";
	response += "Content-Length:" + to_String(_body.size()) + "\n";
	response += "Content-Type:text/html; charset=utf-8 \n";
	response += "\r\n";
	response += _body;
	if (send(_client.get_fd(), response.c_str(), response.size(), 0) < 0)
	{
		_syscall_error = "send()";
		_client.log(_client.log_error(_syscall_error), false);
	}
	_client.log(_client.log_access(_status_code), true);
	return true;
}

void Response::delete_file()
{
	Config conf = _client.get_conf();
	if (_if_location == true)
		_path = _location.get_root() + _client.get_request_target();
	else
		_path = conf.get_root() + _client.get_request_target();

	struct stat sb;
	if (stat(_path.c_str(), &sb) == -1)
	{
		_status_code = 500;
		_syscall_error = "stat() \"" + _path + "\" " + "failed (" + strerror(errno) + ")";
		_client.log(_client.log_error(_syscall_error), false);
	}
	else if (sb.st_mode & S_IFDIR)
		_status_code = 403;
	else if (sb.st_mode & S_IFREG)
	{
		if (remove(_path.c_str()) != 0)
		{
			_status_code = 500;
			_syscall_error = "remove() \"" + _path + "\" " + "failed (" + strerror(errno) + ")";
			_client.log(_client.log_error(_syscall_error), false);
		}
		else
		{
			_status_code = 200;
			_body = "<!DOCTYPE html><html lang=\"en\"><head><title>";
			_body += "DELETED";
			_body += "</title></head><body><center><h1>";
			_body += "File successfully deleted.";
			_body += "</h1></center></body></html>";
		}
	}
}

bool Response::send_cgi_response(std::string body)
{
	std::string response;

	_status_code = 200;
	response = "HTTP/1.1 ";
	response += to_String(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";
	response += _header_fields;
	response += "\r\n";
	response += body;
	if (send(_client.get_fd(), response.c_str(), response.size(), 0) < 0)
	{
		_syscall_error = "send()";
		_client.log(_client.log_error(_syscall_error), false);
	}
	_client.log(_client.log_access(_status_code), true);
	return true;
}

bool Response::send_successful_response()
{
	std::string response;

	response = "HTTP/1.1 ";
	response += to_String(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";
	response += _header_fields;
	response += "\r\n";
	response += _body;
	if (send(_client.get_fd(), response.c_str(), response.size(), 0) < 0)
	{
		_syscall_error = "send()";
		_client.log(_client.log_error(_syscall_error), false);
	}
	_client.log(_client.log_access(_status_code), true);
	return true;
}

std::string Response::get_file_content(std::string content)
{
	std::ifstream input_file(content.c_str());
	if (!input_file.is_open())
	{
		_status_code = 500;
		_syscall_error = "ifstream open() \"" + _path + "\" " + "failed ";
		_client.log(_client.log_error(_syscall_error), false);
		return std::string();
	}
	_status_code = 200;
	return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

void Response::set_body()
{
	struct stat s;

	if (stat(_path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFREG) // Element is a regular file.
			_body = get_file_content(_path);
		else if (s.st_mode & S_IFDIR)
		{ // Element is a directory
			if ((_if_location && _location.get_autoindex() == true) ||
				_conf.get_autoindex() == true)
				set_autoindex_body();
			else
				_status_code = 307;
		}
	}
	else
	{
		_status_code = 500;
		_syscall_error = "stat() \"" + _path + "\" " + "failed (" + strerror(errno) + ")";
		_client.log(_client.log_error(_syscall_error), false);
	}
}

void Response::set_autoindex_body()
{
	DIR *dir;
	struct dirent *entry;
	std::vector<std::string> files;
	if (((dir = opendir(_path.c_str())) != NULL))
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
			{
				files.push_back(std::string(entry->d_name));
			}
		}
		closedir(dir);
	}
	else
	{
		_status_code = 500;
		_syscall_error = "opendir() \"" + _path + "\" " + "failed (" + strerror(errno) + ")";
		_client.log(_client.log_error(_syscall_error), false);
		return;
	}
	_body = "<!DOCTYPE html><html><body>\n"
			"<style>"
			" body {background-color: #353535;}"
			"div {border: solid 1px black; box-sizing: border-box; width: 50%;  height: auto; margin: 0 auto; border-radius: 10px; background-color: #2b2a33 }"
			"p {width:50%;}"
			"a {display: block; padding: 5px 10px; color: #6565af;}"
			"</style>\n<div>\n";
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		_body += "<a href=\"";
		_body += _client.get_request_target();
		if (_client.get_request_target() != "/")
			_body += "/";
		_body += (*it);
		_body += "\">";
		_body += (*it);
		_body += "</a>";
	}
	_body += "</div></body></html>";
	_status_code = 200;
}

bool Response::set_defined_error_page()
{
	if (!_conf.get_error_page(_status_code).empty())
	{
		_path = _client.create_path(_conf.get_error_page(_status_code));
		std::ifstream myfile(_path.c_str());
		if (myfile.is_open())
		{
			std::string myline;
			while (myfile)
			{
				std::getline(myfile, myline);
				_body += myline;
			}
			myfile.close();
			return true;
		}
		_syscall_error = "ifstream open() \"" + _path + "\" " + "failed ";
		_client.log(_client.log_error(_syscall_error), false);
	}
	return false;
}

void Response::set_default_error_page()
{
	_body = "<!DOCTYPE html><html lang=\"en\"><head><title>";
	_body += to_String(_status_code) + " " + get_code_msg();
	_body += "</title></head><body><center><h1>";
	_body += to_String(_status_code) + " " + get_code_msg();
	_body += "</h1></center></body></html>";
}

bool Response::send_error_response()
{
	std::string response;
	_body.clear();
	response = "HTTP/1.1 ";
	response += to_String(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";
	if (set_defined_error_page() == false)
		set_default_error_page();

	response += _header_fields;
	response += "\r\n";
	response += _body;

	if (send(_client.get_fd(), response.data(), response.size(), 0) < 0)
	{
		_syscall_error = "send()";
		_client.log(_client.log_error(_syscall_error), false);
	}
	_client.log(_client.log_access(_status_code), true);
	return true;
}

bool Response::is_redirected()
{
	if (_if_location && !_location.get_return().empty() &&
		_client.get_request_target() != _location.get_return())
		return true;
	else if (!_conf.get_return().empty() && _client.get_request_target() != _conf.get_return())
		return true;
	return false;
}

bool Response::send_response()
{
	if (_status_code)
	{
		// If _status_code is already set with a value greater than zero that means
		// Client class found an error earlier
		// No need to process more, just respond as soon as possible and leave the function
		send_error_response();
		return true;
	}
	if (!_extension.empty() &&
		((_if_location && _location.get_cgi(_extension).first == true) ||
		 _conf.get_cgi(_extension).first == true))
	{
		std::pair<bool, std::string> cgi_body;
		std::string reponse;
		Cgi cgi(_client, _conf, _location);
		std::string script;
		if (_if_location == true)
			script = _location.get_cgi(_extension).second;
		else
			script = _conf.get_cgi(_extension).second;
		if (script.empty())
			_status_code = 500;
		cgi_body = cgi.handler(const_cast<char *>(script.c_str()));
		if (cgi_body.first == false)
			_status_code = 500;
		else
		{
			set_header_fields(cgi_body.second.size());
			return send_cgi_response(cgi_body.second);
		}
	}
	else if (_client.get_method() == "GET")
	{
		if (is_redirected())
			_status_code = 307;
		else
			set_body();
		set_header_fields(_body.size());
	}
	else if (_client.get_method() == "DELETE")
		delete_file();
	else if (_client.get_method() == "POST")
	{
		if (_status_code < 300)
		{
			_status_code = 201;
			return post_body();
		}
	}

	if (_status_code >= 300 && _status_code <= 599)
		return send_error_response();
	return send_successful_response();
}

std::string Response::get_code_msg()
{
	if (_status_code_list.count(_status_code))
		return _status_code_list[_status_code];
	return std::string();
}

void Response::init_code_msg()
{
	_status_code_list[200] = "OK";
	_status_code_list[201] = "Created";
	_status_code_list[202] = "Accepted";
	_status_code_list[203] = "Non-Authoritative Information";
	_status_code_list[204] = "No Content";
	_status_code_list[205] = "Reset Content";
	_status_code_list[206] = "Partial Content";
	_status_code_list[207] = "Multi-Status";
	_status_code_list[208] = "Already Reported";
	_status_code_list[226] = "IM Used";
	_status_code_list[300] = "Multiple Choices";
	_status_code_list[301] = "Moved Permanently";
	_status_code_list[302] = "Found";
	_status_code_list[303] = "See Other";
	_status_code_list[304] = "Not Modified";
	_status_code_list[305] = "Use Proxy";
	_status_code_list[306] = "Switch Proxy";
	_status_code_list[307] = "Temporary Redirect";
	_status_code_list[308] = "Permanent Redirect";
	_status_code_list[400] = "Bad Request";
	_status_code_list[401] = "Unauthorized";
	_status_code_list[402] = "Payment Required";
	_status_code_list[403] = "Forbidden";
	_status_code_list[404] = "Not Found";
	_status_code_list[405] = "Method Not Allowed";
	_status_code_list[406] = "Not Acceptable";
	_status_code_list[407] = "Proxy Authentication Required";
	_status_code_list[408] = "Request Timeout";
	_status_code_list[409] = "Conflict";
	_status_code_list[410] = "Gone";
	_status_code_list[411] = "Length Required";
	_status_code_list[412] = "Precondition Failed";
	_status_code_list[413] = "Payload Too Large";
	_status_code_list[414] = "URI Too Long";
	_status_code_list[415] = "Unsupported Media Type";
	_status_code_list[416] = "Range Not Satisfiable";
	_status_code_list[417] = "Expectation Failed";
	_status_code_list[418] = "I\'m a teapot";
	_status_code_list[421] = "Misdirected Request";
	_status_code_list[422] = "Unprocessable Entity";
	_status_code_list[423] = "Locked";
	_status_code_list[424] = "Failed Dependency";
	_status_code_list[425] = "Too Early";
	_status_code_list[426] = "Upgrade Required";
	_status_code_list[428] = "Precondition Required";
	_status_code_list[429] = "Too Many Requests";
	_status_code_list[431] = "Request Header Fields Too Large";
	_status_code_list[451] = "Unavailable For Legal Reasons";
	_status_code_list[500] = "Internal Server Error";
	_status_code_list[501] = "Not Implemented";
	_status_code_list[502] = "Bad Gateway";
	_status_code_list[503] = "Service Unavailable";
	_status_code_list[504] = "Gateway Timeout";
	_status_code_list[505] = "HTTP Version Not Supported";
	_status_code_list[506] = "Variant Also Negotiates";
	_status_code_list[507] = "Insufficient Storage";
	_status_code_list[508] = "Loop Detected";
	_status_code_list[510] = "Not Extended";
	_status_code_list[511] = "Network Authentication Required";
}

std::string Response::content_mime_type(std::string extension)
{
	std::map<std::string, std::string> type;
	type[".acc"] = "audio/aac";
	type[".abw"] = "application/x-abiword";
	type[".arc"] = "application/x-freearc";
	type[".avif"] = "image/avif";
	type[".avi"] = "video/x-msvideo";
	type[".azw"] = "application/vnd.amazon.ebook";
	type[".bin"] = "application/octet-stream";
	type[".bmp"] = "image/bmp";
	type[".bz"] = "application/x-bzip";
	type[".bz2"] = "application/x-bzip2";
	type[".cda"] = "application/x-cdf";
	type[".csh"] = "application/x-csh";
	type[".css"] = "text/css";
	type[".csv"] = "text/csv";
	type[".doc"] = "application/msword";
	type[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	type[".eot"] = "application/vnd.ms-fontobject";
	type[".epub"] = "application/epub+zip";
	type[".gz"] = "application/gzip";
	type[".gif"] = "image/gif";
	type[".htm"] = "text/html";
	type[".html"] = "text/html";
	type[".ico"] = "image/vnd.microsoft.icon";
	type[".ics"] = "text/calendar";
	type[".jar"] = "application/java-archive";
	type[".jpeg"] = "image/jpeg";
	type[".jpg"] = "image/jpeg";
	type[".js"] = "text/javascript";
	type[".json"] = "application/json";
	type[".jsonld"] = "application/ld+json";
	type[".mid"] = "audio/midi";
	type[".midi"] = "audio/x-midi";
	type[".mjs"] = "text/javascript";
	type[".mp3"] = "audio/mpeg";
	type[".mp4"] = "video/mp4";
	type[".mpeg"] = "video/mpeg";
	type[".mpkg"] = "application/vnd.apple.installer+xml";
	type[".odp"] = "application/vnd.oasis.opendocument.presentation";
	type[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	type[".odt"] = "application/vnd.oasis.opendocument.text";
	type[".oga"] = "audio/ogg";
	type[".ogv"] = "video/ogg";
	type[".ogx"] = "application/ogg";
	type[".opus"] = "audio/opus";
	type[".otf"] = "font/otf";
	type[".png"] = "image/png";
	type[".pdf"] = "application/pdf";
	type[".php"] = "application/x-httpd-php";
	type[".ppt"] = "application/vnd.ms-powerpoint";
	type[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	type[".rar"] = "application/vnd.rar";
	type[".rtf"] = "application/rtf";
	type[".sh"] = "application/x-sh";
	type[".svg"] = "image/svg+xml";
	type[".tar"] = "application/x-tar";
	type[".tif"] = "image/tiff";
	type[".tiff"] = "image/tiff";
	type[".ts"] = "video/mp2t";
	type[".ttf"] = "font/ttf";
	type[".txt"] = "text/plain";
	type[".vsd"] = "application/vnd.visio";
	type[".wav"] = "audio/wav";
	type[".weba"] = "audio/webm";
	type[".webm"] = "video/webm";
	type[".webp"] = "image/webp";
	type[".woff"] = "font/woff";
	type[".woff2"] = "font/woff2";
	type[".xhtml"] = "application/xhtml+xml";
	type[".xls"] = "application/vnd.ms-excel";
	type[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	type[".xml"] = "application/xml";
	type[".xul"] = "application/vnd.mozilla.xul+xml";
	type[".zip"] = "application/zip";
	type[".3gp"] = "video/3gpp";
	type[".3g2"] = "video/3gpp2;";
	type[".7z"] = "application/x-7z-compressed";

	if (type.count(extension) != 0)
		return type[extension];
	return std::string();
}
