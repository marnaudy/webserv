#include "Location.hpp"

Location::Location(std::string name) : 
	_name(name),
	_allowMeth(1, "GET"),
	_autoindex(true),
	_allowUpload(false),
	_returnCode (0) {}

void Location::checkConfig() {
	if (_name.length() == 0 && _name[0] != '/')
		throw BadConfigException("Location name not valid");
	if (_root.length() == 0 && _returnCode == 0 && _returnDest.length() == 0)
		throw BadConfigException("Location missing root");
	if (_allowMeth.size() == 0)
		throw BadConfigException("Location doesn't allow any method");
	if (_allowUpload && _uploadLocation.length() == 0)
		throw BadConfigException("Location doesn't specify upload location");
	for (std::map<std::string, std::string>::iterator it = _cgi.begin(); it != _cgi.end(); ++it) {
		if (it->first[0] != '.')
			throw BadConfigException("CGI directive has no extension");
	}
	if ((_returnCode != 0) != (_returnDest.length() != 0))
		throw BadConfigException("Return directive is incomplete");
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

void Location::parseCGI(std::string value) {
	std::string extension = parseDirective(value);
	std::string binary = parseValue(value);
	_cgi.insert(std::pair<std::string, std::string>(extension, binary));
}

void Location::parse(std::ifstream &ifs) {
	std::string line;
	std::getline(ifs, line);
	while (parseDirective(line) != "end_location") {
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
		} else if (directive == "cgi") {
			parseCGI(parseValue(line));
		} else if (directive == "return_code") {
			_returnCode = atoi(parseValue(line).c_str());
			if (_returnCode < 301 || _returnCode > 308)
				throw BadConfigException("Invalid redirection code");
		} else if (directive == "return_dest") {
			_returnDest = parseValue(line);
		} else {
			throw BadConfigException("Unknown directive in location");
		}
		std::getline(ifs, line);
	}
	checkConfig();
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
	for (std::map<std::string, std::string>::iterator it = _cgi.begin(); it != _cgi.end(); ++it) {
		std::cout << "CGI = " << it->first << ":" << it->second << std::endl;
	}
	std::cout << "returnCode = " << _returnCode << std::endl;
	std::cout << "returnDest = " << _returnDest << std::endl;
}

unsigned int Location::match(std::string &uri) {
	if (uri.find(_name) != 0 && (uri.length() == _name.length() || uri[_name.length()] == '/'))
		return (0);
	return (_name.length());
}

void Location::handleRequest(Request &req, Response &res) {
	if (req.getMethod() != "GET") {
		res.setCode(405);
		return;
	}
	//GET only
	std::string fileName = _root + req.getURI().substr(_name.length());
	std::ifstream ifs(fileName.c_str());
	if (!ifs.is_open() || ifs.fail()) {
		res.setCode(404);
		return;
	}
	res.readFileContent(ifs);
	res.setContentType(fileName);
	ifs.close();
}

std::string parseDirective(std::string &line) {
	size_t start = line.find_first_not_of("\t ");
	size_t end = line.find_first_of(" ", start);
	// if (start == std::string::npos || end == std::string::npos)
	if (start == std::string::npos)
		throw BadConfigException("Bad directive formating");
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
