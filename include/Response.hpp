#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>

class Response {
public:
	Response();
	Response(unsigned int code);
	~Response();
	void setCode(unsigned int code);
	void addHeader(std::string field, std::string value);
	size_t exprt(char **buffer);
private:
	unsigned int _code;
	std::string _message;
	std::map<std::string, std::string> _headers;
	std::vector<char> _content;
};

#endif
