#include "Server.hpp"

int main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	Server serv;
	try {
		serv.loadServer();
		serv.printConfig();
		serv.run(envp);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	serv.closeServer();
}
