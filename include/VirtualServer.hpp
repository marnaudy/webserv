#ifndef VIRTUALSERVER_HPP
#define VIRTUALSERVER_HPP

#include "Location.hpp"
#include "Location.hpp"
#include <algorithm>

class VirtualServer {
public:
	VirtualServer();
	~VirtualServer() {}
	unsigned int getPort();
	u_int32_t getAddress();
	unsigned int getMaxBodySize();
	void parse(std::ifstream &ifs);
	void print();
	void checkConfig();

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

