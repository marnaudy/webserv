#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "webserv.hpp"
#include "VirtualServer.hpp"

class Config {
public:
	Config() {}
	~Config() {}
	void loadConfig(std::string fileName);
	void print();
	std::map<unsigned int,std::list<addressInfo> > getPortList();
	responseCgi handleRequest(Request &req, char **envp, Server *serv);
private:
	void parse(std::ifstream &ifs);
	std::list<VirtualServer> _servers;
};

#endif

