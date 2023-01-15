#include "VirtualServer.hpp"

VirtualServer::VirtualServer() : _port(80), _maxBodySize(2000000) {}

unsigned int VirtualServer::getPort() {
	return (_port);
}

u_int32_t VirtualServer::getAddress() {
	return (_address);
}

void VirtualServer::parseAddress(std::string value) {
	_address = 0;
	for (int i = 3; i >= 0; i--) {
		size_t end = value.find_first_of(".");
		std::string byte = value.substr(0, end);
		if (byte.size() < 1 || byte.size() > 3
			|| byte.find_first_not_of("0123456789") != std::string::npos
			|| atoi(byte.c_str()) > 255)
			throw BadConfigException("Bad address");
		_address += atoi(byte.c_str()) << (i * 8);
		if (end == std::string::npos)
			value.erase(0, end);
		else
			value.erase(0, end + 1);
	}
	if (value.length() != 0) {
		throw BadConfigException("Bad address");
	}
}

void VirtualServer::checkConfig() {
	if (_locations.size() == 0)
		throw BadConfigException("Server has no location");
	if (_port >= 65536)
		throw BadConfigException("Invalid port");
}

void VirtualServer::parseErrorPage(std::string value) {
	size_t startPage = value.find_last_of(" ");
	if (startPage == std::string::npos || startPage == value.length() - 1)
		throw BadConfigException("Bad error_page formating");
	std::string pageName = value.substr(startPage + 1);
	value.erase(startPage);
	size_t end = 0, start = 0;
	while (end != std::string::npos) {
		end = value.find_first_of(" ", start);
		std::string token = value.substr(start, end - start);
		int errorCode = atoi(token.c_str());
		if (errorCode >= 400 && errorCode < 600) {
			if (_errorPages.find(errorCode) != _errorPages.end())
				_errorPages.erase(errorCode);
			_errorPages.insert(std::pair<unsigned int, std::string>(errorCode, pageName));
		} else {
			throw BadConfigException("Bad error_page formating");
		}
		start = end + 1;
	}
}

void VirtualServer::parse(std::ifstream &ifs) {
	std::string line;
	std::getline(ifs, line);
	while (parseDirective(line) != "end_server") {
		if (ifs.eof())
			throw BadConfigException("Unexpected eof");
		std::string directive = parseDirective(line);
		if (directive == "listen_port") {
			_port = atoi(parseValue(line).c_str());
		} else if (directive == "listen_address") {
			parseAddress(parseValue(line));
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
	checkConfig();
}

void VirtualServer::print() {
	std::cout << "port = " << _port << std::endl;
	std::cout << "address = " << std::hex << _address << std::dec << std::endl;
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
