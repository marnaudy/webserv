#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ConfigServer.hpp"

class Config {
public:
	Config() {}
	~Config() {}

	bool isValid();
private:
	std::list<ConfigServer> _servers;
};

#endif

