#include "Location.hpp"

Location::Location(std::string name) : 
	_name(name),
	_allowMeth(1, "GET"),
	_autoindex(false),
	_allowUpload(false),
	_returnCode (0) {}

void Location::checkConfig() {
	if (_name.length() == 0 || _name[0] != '/')
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
	if (uri.find(_name) == 0 && (uri.length() == _name.length()
		|| uri[_name.length()] == '/' || _name[_name.length() - 1] == '/'))
		return (_name.length());
	return (0);
}

void Location::handleGetFile(Response &res, std::string fileName) {
	std::ifstream ifs(fileName.c_str());
	if (!ifs.is_open() || ifs.fail()) {
		res.setCode(404);
		return;
	}
	res.readFileContent(ifs);
	res.setContentType(fileName);
	res.setCode(200);
	ifs.close();
	res.printContent();
}

void Location::handleDirList(Response &res, std::string dirName, std::string &uri) {
	DIR *dirStream = opendir(dirName.c_str());
	if (dirStream == NULL) {
		res.setCode(500);
		return;
	}
	std::string html = "<html>\n<head><title>Index of " + uri + "</title></head>\n<body>\n<h1>Index of ";
	html += uri + "</h1><hr><br>\n";
	struct dirent *dirContent = readdir(dirStream);
	while (dirContent != NULL) {
		html += "<a href=\"";
		html += dirContent->d_name;
		html += "\">";
		html += dirContent->d_name;
		html += "</a><br>\n";
		dirContent = readdir(dirStream);
	}
	html += "<hr></body>\n</html>";
	res.setContent(html);
	res.setCode(200);
	res.setContentType(".html");
	closedir(dirStream);
}

void Location::handleGetDir(Response &res, std::string fileName, std::string &uri) {
	if (_dirPage.length() != 0)
		fileName = _dirPage;
	else {
		struct stat fileStat;
		std::string dirName = fileName;
		fileName += "index.html";
		if (stat(fileName.c_str(), &fileStat) < 0
			|| !S_ISREG(fileStat.st_mode)) {
			if (_autoindex)
				handleDirList(res, dirName, uri);
			else
				res.setCode(403);
			return ;
		}
	}
	handleGetFile(res, fileName);
}

void Location::handleGet(Request &req, Response &res) {
	res.addHeader("cache-control", "no-cache");
	std::string fileName = getFileName(req.getURI());
	std::cout << "GET file : " << fileName << std::endl;
	if (_returnCode != 0) {
		res.setCode(_returnCode);
		res.addHeader("location", _returnDest);
		return;
	}
	struct stat fileStat;
	if (stat(fileName.c_str(), &fileStat) < 0) {
		if (errno == ENOENT || errno == ENOTDIR)
			res.setCode(404);
		else
			res.setCode(403);
		return;
	}
	if (S_ISDIR(fileStat.st_mode)) {
		handleGetDir(res, fileName, req.getURI());
		return;
	}
	handleGetFile(res, fileName);
}

void Location::handlePost(Request &req, Response &res) {
	std::cout << "POST : " << getFileName(req.getURI()) << std::endl;
	if (!_allowUpload) {
		res.setCode(403);
		return;
	}
	std::string length = req.getHeader("content-length");
	if (length.length() == 0 || atoi(length.c_str()) == 0) {
		res.setCode(411);
		return;
	}
	std::string fileName = getFileName(req.getURI());
	std::ofstream ofs(fileName.c_str());
	if (!ofs.is_open() || ofs.fail()) {
		res.setCode(403);
		return;
	}
	ofs.write(req.getContent(), req.getContentSize());
	ofs.close();
	res.setCode(200);
}

void Location::handleDelete(Request &req, Response &res) {
	std::string fileName = getFileName(req.getURI());
	std::cout << "DELETE : " << fileName << std::endl;
	struct stat fileStat;
	if (stat(fileName.c_str(), &fileStat) < 0) {
		res.setCode(404);
		return;
	}
	if (unlink(fileName.c_str()) < 0) {
		res.setCode(403);
		return;
	}
	res.setCode(200);
}

void Location::handleRequest(Request &req, Response &res) {
	if (std::find(_allowMeth.begin(), _allowMeth.end(), req.getMethod()) == _allowMeth.end()) {
		res.setCode(405);
		return;
	}
	if (req.getMethod() == "GET")
		handleGet(req, res);
	if (req.getMethod() == "POST")
		handlePost(req, res);
	if (req.getMethod() == "DELETE")
		handleDelete(req, res);
}

std::string Location::getFileName(std::string &uri) {
	return (_root + "/" + uri.substr(_name.length()));
}

std::string parseDirective(std::string &line) {
	size_t start = line.find_first_not_of("\t ");
	size_t end = line.find_first_of(" ", start);
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
