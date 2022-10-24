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
	ServerConf conf;

	if (argc != 2)
		std::cout << "Usage: " << argv[0] << " configuration_file\n";
	else
		parseFile(argv[1], conf);

	return 0;
}