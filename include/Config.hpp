#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "VirtualServer.hpp"

class Config {
public:
	Config(std::string fileName);
	~Config() {}
	void print();
	std::map<unsigned int,u_int32_t> getPortList();
private:
	void parse(std::ifstream &ifs);
	std::list<VirtualServer> _servers;
};

#endif

