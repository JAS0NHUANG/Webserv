#include "webserv.hpp"

// References:
// - https://www.linode.com/docs/guides/how-to-configure-nginx/
// - https://www.digitalocean.com/community/tools/nginx
// - https://www.plesk.com/blog/various/nginx-configuration-guide/
// - http://kbeezie.com/nginx-configuration-examples/
// - https://doc.ubuntu-fr.org/nginx
// - https://subscription.packtpub.com/book/networking-and-servers/9781785288654/1/ch01lvl1sec09/testing-nginx-configuration
// - https://docs.nginx.com/nginx/admin-guide/web-server/web-server/


// NOTE: Use throw and catch for error (like parsing errors)
int main(int argc, char *argv[])
{
	std::vector<ServerConf> conf;


	(void)argc;
	if (argc != 2)
		std::cout << "Usage: " << argv[0] << " configuration_file\n";
	else {
		try {
			throwParsingError("", "", 0,  argv[0], argv[1]);
			parseFile(argv[1], conf);
			conf.front().debug();
		}
		catch (std::string &str) {
			std::cerr << RED ;
			std::cerr << str << "\n";
			std::cerr << RESET ;
		}
	}

	return 0;
}