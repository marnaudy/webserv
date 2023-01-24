#include "Server.hpp"
#include <signal.h>

bool g_running = true;
bool g_parent = true;

void sigIntHandler(int sig) {
	(void)sig;
	g_running = false;
}

int main(int argc, char **argv, char **envp)
{
	if (argc > 2) {
		std::cout << "Too many arguments (one max)" << std::endl;
		return (1);
	}
	Server serv;
	struct sigaction sa;

	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = &sigIntHandler;
	if (sigaction(SIGINT, &sa, NULL)) {
		std::cout << "Sigaction failed" << std::endl;
		return (-1);
	}
	try {
		if (argc == 2)
			serv.loadServer(argv[1]);
		else
			serv.loadServer();
		serv.printConfig();
		serv.run(envp);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	serv.closeServer();
}
