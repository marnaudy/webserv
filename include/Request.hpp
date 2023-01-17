#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "Buffer.hpp"

class Request {
public:
	Request() {}
	~Request() {}
	int parse(Buffer &buf, unsigned int maxBodySize);
	void print();
private:
	int readContent(Buffer &buf, unsigned int maxBodySize, int contentLength);
	int readContentChunked(Buffer &buf, unsigned int maxBodySize);
	std::string _method;
	std::string _uri;
	std::string _version;
	std::map<std::string, std::string> _headers;
	std::vector<char> _content;
};

#endif
