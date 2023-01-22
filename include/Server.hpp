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
	void run(char **envp);
	void closeServer();
private:
	void openSockets();
	void closeSocket(Socket &sock);
	void dealSocketEvent(Socket *sock, u_int32_t event, char **envp);
	void dealCgiEvent(CgiHandler *cgi, u_int32_t event);
	Config _config;
	std::list<Socket> _sockets;
	int _epfd;
};

#endif
