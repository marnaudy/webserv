#include "Config.hpp"
#include "Response.hpp"
#include <unistd.h>

int main(void)
{
	std::ifstream ifs("webserv.conf");
	Config webserv;
	webserv.parse(ifs);
	webserv.print();
	ifs.close();
	std::cout << std::endl;
	Response response(200);
	response.addHeader("blabla", "prout");
	char *message;
	size_t size = response.exprt(&message);
	write(1, message, size);
	delete[] message;
}
