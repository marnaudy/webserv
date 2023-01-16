#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "Buffer.hpp"

class Request {
public:
	Request() {}
	~Request() {}
	int parse(Buffer &buf);
	void print();
private:
	std::string _method;
	std::string _uri;
	std::string _version;
	std::map<std::string, std::string> _headers;
	std::vector<char> _content;
};

#endif
