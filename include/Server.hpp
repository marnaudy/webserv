#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"
#include <algorithm>

class Server {
public:
	Server();
	~Server() {}
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

