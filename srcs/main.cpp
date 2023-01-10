#include "Config.hpp"

int main(void)
{
	std::ifstream ifs("webserv.conf");
	Config webserv;
	webserv.parse(ifs);
	webserv.print();
	ifs.close();
}
