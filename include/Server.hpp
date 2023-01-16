#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
#include "Socket.hpp"

#define MAX_EVENTS 10

class Server {
public:
	Server() {}
	~Server() {}
	void loadServer(std::string fileName = "webserv.conf");
	void printConfig();
	void run();
	void closeServer();
private:
	void openSockets();
	Config _config;
	std::list<Socket> _sockets;
	int _epfd;
};

#endif
