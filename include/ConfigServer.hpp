#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP

#include "Location.hpp"
#include <map>
#include <algorithm>

class ConfigServer {
public:
	ConfigServer();
	~ConfigServer() {}

	bool isValid();
private:
	std::list<Location> _locations;
	unsigned int _port;
	std::string _address;
	std::list<std::string> _names;
	std::map<unsigned int, std::string> _errorPages;
	unsigned int _maxBodySize;
};

#endif

