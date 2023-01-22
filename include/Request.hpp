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
	Request(unsigned int port, u_int32_t address);
	~Request() {}
	int parse(Buffer &buf, unsigned int maxBodySize);
	void print();
	unsigned int getPort();
	u_int32_t getAddress();
	std::string &getHeader(std::string field);
	std::map<std::string, std::string> &getHeaders();
	std::string &getMethod();
	std::string &getURI();
	std::string &getVersion();
	unsigned int getErrorCode();
	void setErrorCode(unsigned int code);
	char *getContent();
	unsigned int getContentSize();
private:
	int checkMethod(std::string &meth);
	int checkURI(std::string &uri);
	int checkVersion(std::string &version);
	int checkFirstLine(std::string line);
	int parseFirstLine(Buffer &buf);
	int parseHeaderLine(Buffer &buf);
	int parseContent(Buffer &buf, unsigned int maxBodySize);
	int readContent(Buffer &buf, unsigned int maxBodySize, int contentLength);
	int readContentChunked(Buffer &buf, unsigned int maxBodySize);
	std::string _method;
	std::string _uri;
	std::string _version;
	std::map<std::string, std::string> _headers;
	std::vector<char> _content;
	unsigned int _port;
	u_int32_t _address;
	unsigned int _errorCode;
};

#endif
