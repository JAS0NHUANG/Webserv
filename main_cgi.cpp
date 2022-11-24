#include "incs/webserv.hpp"
#include "Cgi.hpp"

int main(int argc, char *argv[])
{
	std::vector<Server> conf;
	std::string body;


	(void)argc;
	if (argc != 2)
		std::cout << "Usage: " << argv[0] << " configuration_file\n";
	else {
		try {
			throwParsingError("", "", argv[0], argv[1]);
			parseFile(argv[1], conf);
			for (std::vector<Server>::iterator it = conf.begin(); it != conf.end(); it++)
				(*it).debug();
			std::cout << YEL << "Number of server : " << conf.size() << "\n" << RESET;
		}
		catch (std::string &str) {
			std::cerr << RED << str << "\n" << RESET;
		}
	}
	// std::cout << "test\n" ;
	// std::pair<bool, std::pair<std::string, std::string> > haha;
	// //haha = conf[0].get_cgi();
	// //std::get<bool>(haha);


	// Request r1;
	// r1.method = 1;
	// r1.path = "/Users/ifeelbored/Documents/42/Webserv/Webserv/index.php";
	// r1.isHttp1_1 = 0;
	// r1.headers["Content-Type"]= "text/html; charset=utf-8";
	// r1.headers["Host"] = "127.0.0.1";
	//get
	Request r1;
	r1.method = 1;
	///Users/ifeelbored/Documents/42/Webserv/Webserv/index.php
	///mnt/nfs/homes/wlo/webserv/Webserv
	r1.path = "/mnt/nfs/homes/wlo/webserv/Webserv/php_get.php";
	//r1.path = "/Users/ifeelbored/Documents/42/Webserv/Webserv/php_get.php";
	r1.isHttp1_1 = 0;
	r1.headers["Content-Type"]= "text/html; charset=utf-8";
	r1.headers["Host"] = "127.0.0.1";
	r1.headers["QUERY_STRING"] = "say=HELLO&to=MOM";

	//post
	Request r2;
	r2.method = 2;
	r2.path = "/Users/ifeelbored/Documents/42/Webserv/Webserv/php_post.php";
	r2.isHttp1_1 = 0;
	r2.body= "name=wlo&email=wlo@gmail.com";
	r2.headers["Content-Type"]= "application/x-www-form-urlencoded";
	r2.headers["Host"] = "127.0.0.1";

	//if get or if post
	std::cerr << "requ.method|" << r1.method <<"|\n";
	Cgi test(r1, conf[0]);
	///usr/lib/cgi-bin/php   php_cgi
	std::string script = "/usr/lib/cgi-bin/php";
	//Cgi test(r2, conf[0]);
	//std::string script = "php_cgi";
	//char *script_char = const_cast<char*>(script.c_str());
	body = test.handler(const_cast<char*>(script.c_str()));
	return 0;
}