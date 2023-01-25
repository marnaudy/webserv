#ifndef VIRTUALSERVER_HPP
#define VIRTUALSERVER_HPP

#include "Location.hpp"
#include "Location.hpp"
#include <algorithm>

class VirtualServer {
public:
	VirtualServer();
	VirtualServer(const VirtualServer &other);
	~VirtualServer() {}
	VirtualServer &operator=(const VirtualServer &rhs);
	unsigned int getPort();
	u_int32_t getAddress();
	unsigned int getMaxBodySize();
	bool matchHost(std::string host);
	void parse(std::ifstream &ifs);
	void print();
	void checkConfig();
	responseCgi handleRequest(Request &req, char**envp, Server *serv);
	void handleError(Response &res);
private:
	void parseErrorPage(std::string value);
	void parseAddress(std::string value);

	std::list<Location> _locations;
	unsigned int _port;
	u_int32_t _address;
	std::list<std::string> _names;
	std::map<unsigned int, std::string> _errorPages;
	unsigned int _maxBodySize;
};

#endif

