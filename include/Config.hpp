#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ConfigServer.hpp"

class Config {
public:
	Config() {}
	~Config() {}
	bool isValid();
	void parse(std::ifstream &ifs);
	void print();
private:
	std::list<ConfigServer> _servers;
};

#endif

