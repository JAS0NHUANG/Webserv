#include "Response.hpp"

void Response::check_setting_location(Config conf){
	//if location exist or 
	std::string path = conf.get_root() + this->client.get_request_target();
	std::cerr << "Path" << path << "\n";
	if (access(path.c_str(), F_OK)  == -1){
		this->status_code = 404;
		return ;
	}
	if (this->client.get_request_target().compare("/") == 0){
		if (conf.get_autoindex == false){
			this->status_code = 404;
			return ;
		}
	}
	//check allow method
	if (location.is_method_allowed(this->client.get_method()) == false){
		this->status_code = 405;
		return ;
	}
	//redicte https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/301
	//https://stackoverflow.com/questions/5781455/how-to-redirect-after-a-successful-delete-request
	if (!(location.get_return().empty())){
		std::cerr << "location.get_return().empty\n";
		if (!(location.get_return_status()).empty())
			this->status_code = toInt(location.get_return_status());
		else if(strcmp(this->client.get_method().c_str(),"GET") == 0)
			this->status_code = 301;
		else if(strcmp(this->client.get_method().c_str(),"POST") == 0)
			this->status_code = 308;
		else if (strcmp(this->client.get_method().c_str(),"DELETE") == 0)
			this->status_code = 303;
		else
			this->status_code = 405;
		return ;
	}
	//if root exist check by access
	//check_cgi 
	if (!this->extension.empty() && this->extension.compare(".html") != 0){
		if (location.get_cgi(this->extension).first == false)
			this->status_code = 401;
	}
}
void Response::set_header_fields(int cont_Leng, Location check_location) {
	Config conf = this->client.get_conf();
	std::map<std::string, std::string> headers;

	headers["Content-Length"] = toString(cont_Leng);
	if (this->status_code >= 400 && this->status_code < 600){
		if(this->status_code == 405){
			std::string tmp;
			if (if_location == true){
				if(check_location.is_method_allowed("GET") == true)
					tmp = "GET, ";
				if(check_location.is_method_allowed("POST") == true)
					tmp += "POST, ";
				if(check_location.is_method_allowed("DELETE") == true)
					tmp += "DELETE, ";
				tmp = tmp.substr(0, tmp.size() - 2);
				headers["Allow"] = tmp;
			} else
				if() 
		}
		headers["Content-Type"] = "text/html; charset=utf-8";
	}
	else if (!this->extension.empty() && this->extension.compare("html") != 0 && check_location.get_cgi(this->extension). first == true)
	{
		headers["Content-Type"] = "text/html; charset=utf-8";
	}
	else{
		if(this->client.get_method() == "GET")
			headers["Content-Type"] = "text/html; charset=utf-8";
		else if (this->client.get_method() == "POST")
			headers["Content-Type"] = "multipart/form-data";//?
		//method delete does not need content-type
		//if multipart/form-data; boundary=something
	}
	if (this->status_code >= 300 && this->status_code < 400)
		headers["Location"] = check_location.get_return();
	//Transfer-Encoding  chunk
	//Keep-Alive
	for( std::map<std::string, std::string>::iterator it=headers.begin(); it!=headers.end(); ++it){
		std::cerr << it->first + "=" + it->second << "\n";
        this->header_fields += it->first;
		this->header_fields += ":";
		this->header_fields += it->second;
		this->header_fields += "\n";
    } 
}

bool Response::post_body(){
	std::string response;

    std::cout << "File successfully saved\n";
	this->body = "<!DOCTYPE html><html><body><p>SAVE THE BODY</P><P>";
	//this->body += this->client.get_body();
	this->body += "</p></body></html>";
	response = "HTTP/1.1 ";
	response += toString(201);
	response += " ";
	response += this->get_code_msg(201);
	response += "\r\n";
	response += "Content-Length:" + toString(this->body.size()) + "\n"; 
	response += "Content-Type:text/html; charset=utf-8 \n";
	response += "\r\n";
	response += this->body;
	std::cerr << "Response :"<< response <<"\n";
	if (send(this->client.get_fd(), response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

bool Response::delete_file(){
	Location location = this->client.get_conf().get_location(this->client.get_request_target()).second;
	std::string path;
	if (this->path.empty())
		path = location.get_root() + this->client.get_request_target();
	else
		path = this->path;
	
	struct stat sb;

	if (stat(path.c_str() , &sb) == -1){
		perror("stat");
		this->status_code = 404;
		return false;
	}
	if (sb.st_mode & S_IFDIR){
		this->status_code = 406;
		std::cerr << "this is a directory not a file\n";
		return false;
	} else if (sb.st_mode & S_IFREG){
		if (remove(path.c_str()) !=  0){
			perror( "Error deleting file" );
			return false;
		}
  		else{
    		std::cout << "File successfully deleted\n";
			this->body = "<!DOCTYPE html><html><body><p>File successfully deleted.</p></body></html>";
		}
	}
	return true;
}

bool Response::send_cgi_response(std::string body) const  {

	std::string response;

	response = "HTTP/1.1 ";
	response += toString(200);
	response += " ";
	response += this->get_code_msg(200);
	response += "\r\n";
	response += this->header_fields;
	response += "\r\n";
	response += body;
	std::cerr << "Response :"<< response <<"\n";
	if (send(this->client.get_fd(), response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}
bool Response::send_successful_response()  {

	std::string response;

	response = "HTTP/1.1 ";
	response += toString(this->status_code);
	response += " ";
	response += this->get_code_msg(this->status_code);
	response += "\r\n";
	response += this->header_fields;
	response += "\r\n";
	response += this->body;
	std::cerr << "Response :" << response <<"\n";
	if (send(this->client.get_fd(), response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

bool Response::set_body(){
	Location location = this->client.get_conf().get_location(this->client.get_request_target()).second;
	std::string path;
	std::cerr << "this->path" << this->path << "\n";
	if (this->path.empty())
		path = location.get_root() + this->client.get_request_target();
	else
		path = this->path;
	std::cerr << "this->path" << this->path << "\n";
	//std::cerr << "path" << path << ", " << this->client.get_request_target() << "\n";
	std::string myline;

	if (this->client.get_request_target().compare("/") == 0 ){
		if (!location.get_index().empty())
			path += location.get_index()[0];
	}
	std::cerr << "path.c_str()222:" << path.c_str() << "\n";
	std::ifstream myfile (path.c_str());
	if (myfile.is_open()){
		while(myfile){
			std::getline (myfile, myline);
			this->body += myline;
		}
	myfile.close();
	}else {
		std::cerr << "Error opening file\n";
		this->status_code = 500;
		return false;
	}
	return true;
	//std::cerr << "body :" <<this->body << "\n";
}

bool Response::set_autoindex_body(){
	std::cout << "set_autoindex_body\n";
	Location location = this->client.get_conf().get_location(this->client.get_request_target()).second;
	std::string path = location.get_root() + this->client.get_request_target();
	DIR *dir;
  	struct dirent *entry;
	std::vector<std::string> files;
	if (((dir = opendir(path.c_str())) != NULL)){
		while((entry = readdir(dir)) != NULL){
			if (std::string(entry->d_name) != "." && std::string(entry->d_name) != ".."){
				files.push_back(std::string(entry->d_name));
				std::cout<<  "entry->d_name :" << entry->d_name << "\n";
			}
		}
		closedir(dir);
	} else{
		std::cerr << "Error opendir\n";
		this->status_code = 500;
		return false;
	}
	body = "<!DOCTYPE html><html><body>";
	for(std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it){
		body += "<h5> <a href=\"";
		body += (*it);
		body += "\">";
		body += (*it);
		body += "</a></h5>";
	}
	body += "</body></html>";
	return true;
}

bool Response::send_error_response(Location location) {

	std::string response;
	std::string body;
	std::string path;


	response = "HTTP/1.1 ";
	response += toString(this->status_code);
	response += " ";
	response += this->get_code_msg(this->status_code);
	response += "\r\n";

	if (!this->client.get_conf().get_error_page(this->status_code).empty())
		path = this->client.get_conf().get_error_page(this->status_code) + "/" + toString(this->status_code) + ".html";
	std::ifstream myfile (path.c_str());
	if (myfile.is_open()){
		std::string myline;
		while(myfile){
	 		std::getline (myfile, myline);
	 		body += myline;
	 	}
		myfile.close();
	}else{
		body = "<!DOCTYPE html><html lang=\"en\"><head><title>";
		body += toString(this->status_code) + " " + this->get_code_msg(this->status_code);
		body += "</title></head><body><center><h1>";
		body += toString(this->status_code) + " " + this->get_code_msg(this->status_code);
		body +="</h1></center></body></html>";
	}
	set_header_fields(body.size(), location);
	response += this->header_fields;
	response += "\r\n";
	response += body;

	std::cerr << "ERROR response :" <<response << "\n";
	if (send(this->client.get_fd(), response.c_str(), response.size(), 0) < 0)
		errMsgErrno("send failed");
	return true;
}

Response::Response(Client client): client(client){

	std::cerr << "get_request_target :" << client.get_request_target()<< "\n";
	std::cerr << "get_path2 :" << client.get_path2()<< "\n";
	std::size_t found = client.get_request_target().find(".");
	if (found!=std::string::npos){
		this->extension = client.get_request_target();
		this->extension = this->extension.substr(found);
		std::cout << "this->extension :|" << this->extension << "|\n";
	}
	this->status_code_list = init_code_msg();
	this->http_version = "HTTP/1.1";
	this->status_code = 0;
	Config conf = this->client.get_conf();
	if (conf.get_location(this->client.get_request_target()).first == false){
		this->location.set_return(conf.get_return());
		this->location.set_return_status(conf.get_return_status());
		this->location.set_root(conf.get_root());
		this->location.set_autoindex(conf.get_autoindex());
		this->location.set_index(conf.get_index());
		this->location.set_upload_store(conf.get_upload_store());
		//if (conf.is_method_allowed("GET") == true)
		//cgi
	}
	else
		this->location = conf.get_location(this->client.get_request_target()).second;
}
Response::~Response(void){

}

bool Response::send_response(){
	Config conf = this->client.get_conf();

	std::cout << "Sending response \n";
	this->check_setting_location(conf);
	//conf.debug();
	if(!this->extension.empty() && this->extension.compare(".html") != 0 && this->status_code == 0) {
		std::pair<bool, std::string> cgi_body;
		std::string reponse;
		//location is cgi;
		Cgi test_cgi(this->client, conf);
		std::string script;
		if (if_location == true)
			script = check_location.get_cgi(this->extension).second;
		else 
			script = conf.get_cgi(this->extension).second;
		if (script.empty())
			this->status_code = 500;
		//std::cerr << "here\n";
		cgi_body = test_cgi.handler(const_cast<char*>(script.c_str()));
		if (cgi_body.first == false){
			this->status_code = 500;
		}else {
			set_header_fields(cgi_body.second.size(), check_location);
			return send_cgi_response(cgi_body.second);
		}
	}else if(this->client.get_method() == "GET" && this->status_code == 0){
		if (check_location.get_autoindex() == true){
			set_autoindex_body();
		} else {
			set_body();
		}
		if (!this->status_code){
			set_header_fields(this->body.size(), check_location);
			this->status_code = 200;
		}
	}else if (this->client.get_method() == "DELETE" && this->status_code == 0){
			if (delete_file() == false && this->status_code == 0)
				this->status_code = 501;
			else if (!this->status_code)
				this->status_code = 202;
	}else if (this->client.get_method() == "POST" && this->status_code == 0){
			this->status_code = 201;
			return post_body();
	}
	if (this->status_code >= 300&& this->status_code <= 599)
		return send_error_response(check_location);
	return send_successful_response();
	//return true; when return false? keep connection alive
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
