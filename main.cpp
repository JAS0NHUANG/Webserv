#include "webserv.hpp"

// References:
// - https://www.linode.com/docs/guides/how-to-configure-nginx/
// - https://www.digitalocean.com/community/tools/nginx
// - https://www.plesk.com/blog/various/nginx-configuration-guide/
// - http://kbeezie.com/nginx-configuration-examples/
// - https://doc.ubuntu-fr.org/nginx
// - https://subscription.packtpub.com/book/networking-and-servers/9781785288654/1/ch01lvl1sec09/testing-nginx-configuration
// - https://docs.nginx.com/nginx/admin-guide/web-server/web-server/

void parseFile(char *fileName)
{
	std::ifstream configFile(fileName);
	if (!configFile.is_open())
	{
		std::cerr << RED;
		std::cerr << "Unable to open configuration file\n";
		std::cerr << RESET;
		return;
	}

	std::string line;
	while (getline(configFile, line))
		std::cout << line;

	configFile.close();
}

int main(int argc, char *argv[])
{
	ServerConf conf;

	std::vector<std::string> vec(splitLine("Ceci   \t est \t\tune\tst#ring\r   de     test."));
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		std::cout << "tmp = |" << *it << "|\n";

	vec = deleteComment(vec);

	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		std::cout << "tmp = |" << *it << "|\n";

	// conf.debug();

	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " configuration_file\n";
	}
	else
		parseFile(argv[1]);

	return 0;
}