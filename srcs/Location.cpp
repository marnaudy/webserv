#include "Location.hpp"

Location::Location(std::string &name) : 
	_name(name),
	_allowMeth(1, "GET"),
	_autoindex(true),
	_allowUpload(false) {}

bool Location::isValid() {
	return (_name.length() != 0
		&& _name[0] == '/'
		&& _root.length() != 0
		&& _allowMeth.size() != 0
		&& (_allowUpload || _uploadLocation.length() != 0));
}

void Location::parseMeth(std::string value) {
	_allowMeth.clear();
	size_t end = 0, start = 0;
	while (end != std::string::npos) {
		end = value.find_first_of(" ", start);
		std::string token = value.substr(start, end - start);
		if (token == "GET") {
			_allowMeth.push_back("GET");
		} else if (token == "POST") {
			_allowMeth.push_back("POST");
		} else if (token == "DELETE") {
			_allowMeth.push_back("DELETE");
		} else {
			throw BadConfigException("Unknown method in limit_except directive");
		}
		start = end + 1;
	}
}

void Location::parse(std::ifstream &ifs) {
	std::string line;
	std::getline(ifs, line);
	while (line != "\tend_location") {
		if (ifs.eof())
			throw BadConfigException("End of location not found");
		std::string directive = parseDirective(line);
		if (directive == "root") {
			_root = parseValue(line);
		} else if (directive == "limit_except") {
			parseMeth(parseValue(line));
		} else if (directive == "autoindex") {
			if (parseValue(line) == "on")
				_autoindex = true;
			else
				_autoindex = false;
		} else if (directive == "dir_page") {
			_dirPage = parseValue(line);
		} else if (directive == "allow_upload") {
			if (parseValue(line) == "yes")
				_allowUpload = true;
			else
				_allowUpload = false;
		} else if (directive == "upload_location") {
			_uploadLocation = parseValue(line);
		} else {
			throw BadConfigException("Unknown directive in location" + line);
		}
		std::getline(ifs, line);
	}
}

void Location::print() {
	std::cout << "name = " << _name << std::endl;
	std::cout << "root = " << _root << std::endl;
	for (std::list<std::string>::iterator it = _allowMeth.begin(); it != _allowMeth.end(); it++) {
		std::cout << "meth = " << *it << std::endl;
	}
	std::cout << "autoindex = " << _autoindex << std::endl;
	std::cout << "dirPage = " << _dirPage << std::endl;
	std::cout << "allowUpload = " << _allowUpload << std::endl;
	std::cout << "uploadLocation = " << _uploadLocation << std::endl;
}

std::string parseDirective(std::string &line) {
	size_t start = line.find_first_not_of("\t ");
	size_t end = line.find_first_of(" ", start);
	// if (start == std::string::npos || end == std::string::npos)
	if (start == std::string::npos)
		throw BadConfigException("Bad directive formating" + line);
	return (line.substr(start, end - start));
}

std::string parseValue(std::string &line) {
	size_t dirStart = line.find_first_not_of("\t ");
	size_t start = line.find_first_of(" ", dirStart);
	if (dirStart == std::string::npos || start == std::string::npos || line.length() == start + 1)
		throw BadConfigException("Directive missing value");
	return (line.substr(start + 1));
}

BadConfigException::BadConfigException(std::string message) : _message(message) {}

const char *BadConfigException::what() const throw() {
	return (_message.c_str());
}
