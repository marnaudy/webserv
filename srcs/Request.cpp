#include "Request.hpp"

std::vector<std::string> split(std::string str) {
	size_t end = 0;
	size_t start = 0;
	std::vector<std::string> list;
	while (end != std::string::npos) {
		end = str.find_first_of(" ", start);
		list.push_back(str.substr(start, end - start));
		start = end + 1;
	}
	return (list);
}

std::string parseHeaderField(std::string &line) {
	size_t end = line.find_first_of(" ");
	return (line.substr(0, end));
}

std::string parseHeaderValue(std::string &line) {
	size_t start = line.find_first_of(" ", 0);
	if (start == std::string::npos || line.length() == start + 1)
		return (std::string());
	return (line.substr(start + 1));
}

int Request::parse(Buffer &buf) {
	buf.setPos(0);
	int status;
	std::string line = buf.getLine(status);
	if (status < 0)
		return (0);
	std::vector<std::string> list = split(line);
	if (list.size() != 3)
		return (-1);
	_method = list[0];
	_uri = list[1];
	_version = list[2];
	line = buf.getLine(status);
	while (status == 0 && line.length() != 0) {
		std::string field = parseHeaderField(line);
		if (field[field.length() - 1] != ':') {
			line = buf.getLine(status);
			continue;
		}
		field.erase(field.length() - 1);
		std::string value = parseHeaderValue(line);
		if (value.length() == 0) {
			line = buf.getLine(status);
			continue;
		}
		if (_headers.find(field) != _headers.end())
			_headers.erase(field);
		_headers.insert(std::pair<std::string, std::string>(field, value));
		line = buf.getLine(status);
	}
	if (status != 0)
		return (0);
	return (buf.getPos());
}

void Request::print() {
	std::cout << "Request" << std::endl;
	std::cout << "Method = " << _method << std::endl;
	std::cout << "uri = " << _uri << std::endl;
	std::cout << "version = " << _version << std::endl;
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
}
