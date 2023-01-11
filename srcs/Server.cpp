#include "Server.hpp"

Server::Server() : _port(80), _address("0.0.0.0"), _maxBodySize(2000000) {}

bool Server::isValid() {
	for (std::list<Location>::iterator it = _locations.begin(); it != _locations.end(); ++it) {
		if (!it->isValid())
			return (false);
	}
	return (_locations.size() != 0);
}

void Server::parseErrorPage(std::string value) {
	size_t startPage = value.find_last_of(" ");
	if (startPage == std::string::npos || startPage == value.length() - 1)
		throw BadConfigException("Bad error_page formating 1");
	std::string pageName = value.substr(startPage + 1);
	value.erase(startPage);
	size_t end = 0, start = 0;
	while (end != std::string::npos) {
		end = value.find_first_of(" ", start);
		std::string token = value.substr(start, end - start);
		int errorCode = atoi(token.c_str());
		if (errorCode >= 400 && errorCode < 600) {
			_errorPages.insert(std::pair<unsigned int, std::string>(errorCode, pageName));
		} else {
			throw BadConfigException("Bad error_page formating 2");
		}
		start = end + 1;
	}
}

void Server::parse(std::ifstream &ifs) {
	std::string line;
	std::getline(ifs, line);
	while (line != "end_server") {
		if (ifs.eof())
			throw BadConfigException("Unexpected eof");
		std::string directive = parseDirective(line);
		if (directive == "listen_port") {
			_port = atoi(parseValue(line).c_str());
		} else if (directive == "listen_address") {
			_address = parseValue(line);
		} else if (directive == "server_name") {
			_names.push_back(parseValue(line));
		} else if (directive == "client_max_body_size") {
			_maxBodySize = atoi(parseValue(line).c_str());
		} else if (directive == "error_page") {
			parseErrorPage(parseValue(line));
		} else if (directive == "location") {
			std::string name = parseValue(line);
			Location newLoc(name);
			newLoc.parse(ifs);
			_locations.push_back(newLoc);
		} else {
			throw BadConfigException("Unknown server directive");
		}
		std::getline(ifs, line);
	}
}

void Server::print() {
	std::cout << "port = " << _port << std::endl;
	std::cout << "address = " << _address << std::endl;
	for (std::list<std::string>::iterator it = _names.begin(); it != _names.end(); it++) {
		std::cout << "name = " << *it << std::endl;
	}
	for (std::map<unsigned int, std::string>::iterator it = _errorPages.begin(); it != _errorPages.end(); ++it) {
		std::cout << "error page = " << it->first << ":" << it->second << std::endl;
	}
	std::cout << "maxSize = " << _maxBodySize << std::endl;
	for (std::list<Location>::iterator it = _locations.begin(); it != _locations.end(); it++) {
		std::cout << "location " << std::endl;
		it->print();
	}
}
