#include "Response.hpp"

void Response::check_setting_location(Config conf)
{
	struct stat info;
	if (stat(_path.c_str(), &info) != 0)
	{
		_status_code = 404;
		return;
	}
	// check allow method
	if (_location.is_method_allowed(_client.get_method()) == false ||
		(_if_location == false && conf.is_method_allowed(_client.get_method()) == false))
	{
		_status_code = 405;
		return;
	}
	// redicte https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/301
	// https://stackoverflow.com/questions/5781455/how-to-redirect-after-a-successful-delete-request
	if (!(_location.get_return().empty()) || (_if_location == false && !conf.get_return().empty()))
	{
		if (!(_location.get_return_status()).empty())
			_status_code = toInt(_location.get_return_status());
		else if (!conf.get_return_status().empty())
			_status_code = toInt(conf.get_return_status());
		else if (strcmp(_client.get_method().c_str(), "GET") == 0)
			_status_code = 301;
		else if (strcmp(_client.get_method().c_str(), "POST") == 0)
			_status_code = 308;
		else if (strcmp(_client.get_method().c_str(), "DELETE") == 0)
			_status_code = 303;
		else
			_status_code = 405;
		return;
	}
	// if root exist check by access
}
void Response::set_header_fields(int cont_Leng)
{
	Config conf = _client.get_conf();
	std::map<std::string, std::string> headers;

	headers["Content-Length"] = toString(cont_Leng);
	if (_status_code >= 400 && _status_code < 600)
	{
		if (_status_code == 405)
		{
			std::string tmp;
			if (_if_location == true)
			{
				if (_location.is_method_allowed("GET") == true || (_if_location == false && conf.is_method_allowed("GET") == true))
					tmp = "GET, ";
				if (_location.is_method_allowed("POST") == true || (_if_location == false && conf.is_method_allowed("POST") == true))
					tmp += "POST, ";
				if (_location.is_method_allowed("DELETE") == true || (_if_location == false && conf.is_method_allowed("DELETE") == true))
					tmp += "DELETE, ";
				tmp = tmp.substr(0, tmp.size() - 2);
				headers["Allow"] = tmp;
			}
		}
	}
	if (!_extension.empty() && _extension.compare(".php") && _extension.compare(".py"))
		headers["Content-Type"] = content_mime_type(_extension);
	else
		headers["Content-Type"] = "text/html; charset=utf-8";
	if (_status_code >= 300 && _status_code < 400 && _if_location == true)
		headers["Location"] = _location.get_return();
	else if (_status_code >= 300 && _status_code < 400 && _if_location == false)
		headers["Location"] = conf.get_return();
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	{
		_header_fields += it->first;
		_header_fields += ":";
		_header_fields += it->second;
		_header_fields += "\n";
	}
}

bool Response::post_body()
{
	std::string response;

	std::cout << "File successfully saved\n";
	_status_code = 201;
	_body = "<!DOCTYPE html><html><body><p>File/data successfully saved</P><P>";
	// body += _client.get_body();
	_body += "</p></body></html>";
	response = "HTTP/1.1 ";
	response += toString(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";
	response += "Content-Length:" + toString(_body.size()) + "\n";
	response += "Content-Type:text/html; charset=utf-8 \n";
	response += "\r\n";
	response += _body;
	if (send(_client.get_fd(), response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

bool Response::delete_file()
{
	Config conf = _client.get_conf();
	if (_if_location == true)
		_path = _location.get_root() + _client.get_request_target();
	else
		_path = conf.get_root() + _client.get_request_target();

	struct stat sb;
	if (stat(_path.c_str(), &sb) == -1)
	{
		std::cerr << "stat"
				  << "\n";
		_status_code = 404;
		return false;
	}
	if (sb.st_mode & S_IFDIR)
	{
		_status_code = 406;
		std::cerr << "this is a directory not a file\n";
		return false;
	}
	else if (sb.st_mode & S_IFREG)
	{
		if (remove(_path.c_str()) != 0)
		{
			std::cerr << "Error deleting file\n";
			return false;
		}
		else
		{
			std::cout << "File successfully deleted\n";
			_body = "<!DOCTYPE html><html><body><p>File successfully deleted.</p></body></html>";
		}
	}
	return true;
}

bool Response::send_cgi_response(std::string body)
{

	std::string response;

	_status_code = 200;
	response = "HTTP/1.1 ";
	response += toString(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";
	response += _header_fields;
	response += "\r\n";
	response += body;
	if (send(_client.get_fd(), response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

bool Response::send_successful_response()
{

	std::string response;

	response = "HTTP/1.1 ";
	response += toString(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";
	response += _header_fields;
	response += "\r\n";
	response += _body;
	if (send(_client.get_fd(), response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

std::string Response::get_file_content(std::string content)
{
	std::ifstream input_file(content.c_str());
	if (!input_file.is_open()) {
		_status_code = 500;
		return std::string();
	}
	return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

bool Response::set_body()
{
	struct stat s;

	if (stat(_path.c_str(), &s) == 0) {
		if (s.st_mode & S_IFREG) // ELement is a regular file.
			_body = get_file_content(_path);
		else if (s.st_mode & S_IFDIR ) { // Element is a directory
			if (_if_location && _location.get_autoindex() == true)
				set_autoindex_body();
			else if (_client.get_conf().get_autoindex() == true)
				set_autoindex_body();
			else {
				_status_code = 403;
				send_error_response();
			}
		}
	}

	return true;
}

bool Response::set_autoindex_body()
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
		std::cerr << "Error opendir\n";
		_status_code = 403;
		return false;
	}
	_body =	"<!DOCTYPE html><html><body>\n"
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
	return true;
}

bool Response::send_error_response()
{

	std::string response;

	_body.clear();
	response = "HTTP/1.1 ";
	response += toString(_status_code);
	response += " ";
	response += get_code_msg();
	response += "\r\n";

	if (!_client.get_conf().get_error_page(_status_code).empty())
		_path = _client.get_conf().get_error_page(_status_code) + "/" + toString(_status_code) + ".html";
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
	}
	else
	{
		_body = "<!DOCTYPE html><html lang=\"en\"><head><title>";
		_body += toString(_status_code) + " " + get_code_msg();
		_body += "</title></head><body><center><h1>";
		_body += toString(_status_code) + " " + get_code_msg();
		_body += "</h1></center></body></html>";
	}
	set_header_fields(_body.size());
	response += _header_fields;
	response += "\r\n";
	response += _body;

	if (send(_client.get_fd(), response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

Response::Response(Client client) : _client(client)
{
	_path = _client.get_path();
	std::string tmp = _client.get_request_target();
	std::size_t found = tmp.find(".");
	while (found != std::string::npos)
	{
		tmp.erase(0, found + 1);
		_extension = "." + tmp;
		found = tmp.find(".");
	}
	init_code_msg();
	_status_code = 0;
	Config conf = _client.get_conf();
	if (conf.get_location(_client.get_request_target()).first == false)
	{
		_if_location = false;
	}
	else
	{
		_location = conf.get_location(_client.get_request_target()).second;
		_if_location = true;
	}
}

Response::~Response(void)
{
}

bool Response::send_response()
{
	Config conf = _client.get_conf();

	std::cout << "Sending response \n";
	check_setting_location(conf);
	if (!_extension.empty() && _status_code == 0 &&
		(_location.get_cgi(_extension).first == true || conf.get_cgi(_extension).first == true))
	{
		std::pair<bool, std::string> cgi_body;
		std::string reponse;
		Cgi test_cgi(_client, conf);
		std::string script;
		if (_if_location == true)
			script = _location.get_cgi(_extension).second;
		else
			script = conf.get_cgi(_extension).second;
		if (script.empty())
			_status_code = 500;
		cgi_body = test_cgi.handler(const_cast<char *>(script.c_str()));
		if (cgi_body.first == false)
		{
			_status_code = 500;
		}
		else
		{
			set_header_fields(cgi_body.second.size());
			return send_cgi_response(cgi_body.second);
		}
	}
	else if (_client.get_method() == "GET" && _status_code == 0)
	{
		set_body();
		if (!_status_code)
		{
			set_header_fields(_body.size());
			_status_code = 200;
		}
	}
	else if (_client.get_method() == "DELETE" && _status_code == 0)
	{
		if (delete_file() == false && _status_code == 0)
			_status_code = 501;
		else if (!_status_code)
			_status_code = 202;
	}
	else if (_client.get_method() == "POST" && _status_code == 0)
	{
		_status_code = 201;
		return post_body();
	}
	if (_status_code >= 300 && _status_code <= 599)
		return send_error_response();
	return send_successful_response();
	// return true; when return false? keep connection alive
}

std::string Response::get_code_msg() const
{
	return _status_code_list.find(_status_code)->second;
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

	for (std::map<std::string, std::string>::iterator it = type.begin(); it != type.end(); it++)
	{
		if (it->first.compare(extension) == 0)
			return it->second;
	}
	return std::string();
}