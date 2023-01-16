#include "Server.hpp"

int main(void)
{
	Server serv;
	try {
		serv.loadServer();
		serv.printConfig();
		serv.run();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	serv.closeServer();
}
