#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "webserv.hpp"
#include "VirtualServer.hpp"

class Config {
public:
	Config() {}
	Config(std::string fileName);
	~Config() {}
	void print();
	std::map<unsigned int,std::list<addressInfo> > getPortList();
private:
	void parse(std::ifstream &ifs);
	std::list<VirtualServer> _servers;
};

#endif

