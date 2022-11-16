#include "./incs/webserv.hpp"

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
	(void)argc;
	std::vector<Server> conf;
	std::vector<Server>::iterator it;

	// get configuration info
	try {
		throwParsingError("", "", argv[0], argv[1]);
		parseFile(argv[1], conf);
		for (it = conf.begin(); it != conf.end(); it++)
			(*it).debug();
		std::cout << YEL << "Number of server : " << conf.size() << "\n" << RESET;
	}
	catch (std::string &str) {
		std::cerr << RED << str << "\n" << RESET;
	}

	// create sockets
	std::vector<Socket> socket_list;
	for (it = conf.begin(); it != conf.end(); it++) {
		std::cout << GRN <<  "\naddress: " << (*it).get_address() << ", port: " << (*it).get_port() << RESET;
		socket_list.push_back(Socket((*it).get_port(), (*it).get_address()));
	}
	std::cout << GRN << "\nSockets created!\n" << RESET;

	// run epoll
	run_epoll(socket_list);

	return (0);
}
