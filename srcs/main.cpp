#include "Server.hpp"

int main(void)
{
	try {
		Server serv;
		serv.printConfig();
		serv.run();
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
