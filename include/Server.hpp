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
	void closeFds();
private:
	void openSockets();
	void closeSocket(Socket &sock);
	int dealSocketEvent(Socket *sock, u_int32_t event, char **envp);
	int dealCgiEvent(CgiHandler *cgi, u_int32_t event);
	Config _config;
	std::list<Socket> _sockets;
	int _epfd;
};

#endif
