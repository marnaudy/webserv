#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "VirtualServer.hpp"

class Config {
public:
	Config() {}
	~Config() {}
	void parse(std::ifstream &ifs);
	void print();
private:
	std::list<VirtualServer> _servers;
};

#endif

