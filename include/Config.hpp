#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"

class Config {
public:
	Config() {}
	~Config() {}
	void parse(std::ifstream &ifs);
	void print();
private:
	std::list<Server> _servers;
};

#endif

