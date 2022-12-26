#include "webserv.hpp"

std::sig_atomic_t g_shutdown = 1;

void	signal_handler(int signal) {
	if (signal == SIGINT)
		g_shutdown = 0;
}

void init_virtual_servers(std::map<std::string, std::vector<Config> > &virtual_servers, 
	std::vector<Socket> &socket_list) {
	// create sockets
	std::map<std::string, std::vector<Config> >::iterator it = virtual_servers.begin();
	std::cout << GRN << "Virtual servers set:" << RESET << std::endl;
	int i = 1;
	for (; it != virtual_servers.end(); it++) {
		std::cout << GRN << i << ". "<< it->second.front().get_address() << ":"
				<< it->second.front().get_port() << "\n" RESET;
		socket_list.push_back(Socket(it->second.front().get_port(),
			it->second.front().get_address(), it->second));
		++i;
	}
}

int main(int argc, char *argv[])
{
	std::signal(SIGINT, signal_handler);
	if (argc != 2) {
		std::cerr << RED << "Configuration file is missing. Please give it as first parameter.\n" << RESET;
		return 1;
	}

	std::vector<Socket> socket_list;
	std::map<std::string, std::vector<Config> > virtual_servers;

	try {
		// get configuration info
		throw_parsing_error("", "", argv[0], argv[1]);
		throwError("", argv[0]);
		parse_file(argv[1], virtual_servers);

		init_virtual_servers(virtual_servers, socket_list);

		// run server
		run_server(socket_list);
	}
	catch (std::string &str) {
		std::cerr << BRED << str << "\n" << RESET;
		return 1;
	}
	std::cout << BGRN " -- Webserv exited succesfully. --\n" RESET;
	return (0);
}
