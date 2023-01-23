#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>

class Response {
public:
	Response();
	Response(unsigned int code);
	~Response();
	void setCode(unsigned int code);
	unsigned int getCode();
	void printContent();
	void addHeader(std::string field, std::string value);
	void addHeaderLine(std::string line);
	void readFileContent(std::ifstream &ifs);
	void setContent(std::string &str);
	void setContent(char *buf, size_t size);
	void setContentType(std::string fileName);
	size_t exprt(char **buffer);
private:
	unsigned int _code;
	std::string _message;
	std::map<std::string, std::string> _headers;
	std::vector<std::string> _headerLines;
	std::vector<char> _content;
};

#endif
