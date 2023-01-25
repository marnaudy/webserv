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
	std::vector<std::string> contentVec;
	while (dirContent != NULL) {
		std::string name;
		name = dirContent->d_name;
		if (dirContent->d_type == DT_DIR)
			name += "/";
		contentVec.push_back(name);
		dirContent = readdir(dirStream);
	}
	std::sort(contentVec.begin(), contentVec.end());
	for (std::vector<std::string>::iterator it = contentVec.begin(); it != contentVec.end(); ++it) {
		html += "<a href=\"";
		html += *it;
		html += "\">";
		html += *it;
		html += "</a><br>\n";
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

responseCgi Location::handleCgi(Request &req, char **envp, Server *serv) {
	responseCgi ret;
	ret.cgi = new CgiHandler;
	ret.isResponse = false;
	std::string ext = getCgiExt(req.getURI());
	ret.cgi->setServAddr(_servAddr);
	ret.cgi->parseUri(req.getURI(), ext);
	std::string script = ret.cgi->getScriptName();
	ret.cgi->setScriptPath(getFileName(script));
	ret.cgi->setBin(_cgi[ext]);
	ret.cgi->addBody(req.getContent(), req.getContentSize());
	int error = ret.cgi->checkCgi();
	if (error != 0) {
		delete ret.cgi;
		ret.isResponse = true;
		ret.response = new Response(error);
		return (ret);
	}
	try {
		ret.cgi->exec(req, envp, serv);
	} catch (std::exception &e) {
		if (!g_parent)
			g_running = false;
		std::cerr << "Cgi error: " << e.what() << std::endl;
		delete ret.cgi;
		ret.isResponse = true;
		ret.response = new Response(500);
	}
	return (ret);
}

std::string Location::getCgiExt(std::string &uri) {
	std::vector<std::string> splitUri = split(uri, '/');
	for (std::vector<std::string>::iterator it = splitUri.begin(); it != splitUri.end(); ++it) {
		size_t end = it->length();
		std::string ext;
		do {
			end = it->find_last_of("?", end - 1);
			size_t begin = it->find_last_of(".", end);
			if (begin != std::string::npos) {
				ext = it->substr(begin);
				if (_cgi[ext].length() != 0)
					return (ext);
			}
		} while (end != std::string::npos && end != 0);
	}
	return ("");
}

responseCgi Location::handleGet(Request &req, char **envp, Server *serv) {
	std::string fileName = getFileName(req.getURI());
	std::cout << "GET file : " << fileName << std::endl;
	responseCgi ret;
	if (getCgiExt(req.getURI()).length() != 0)
		return (handleCgi(req, envp, serv));
	ret.isResponse = true;
	ret.response = new Response;
	ret.response->addHeader("cache-control", "no-cache");
	if (_returnCode != 0) {
		ret.response->setCode(_returnCode);
		ret.response->addHeader("location", _returnDest);
		return (ret);
	}
	struct stat fileStat;
	if (stat(fileName.c_str(), &fileStat) < 0) {
		if (errno == ENOENT || errno == ENOTDIR)
			ret.response->setCode(404);
		else
			ret.response->setCode(403);
		return (ret);
	}
	if (S_ISDIR(fileStat.st_mode)) {
		handleGetDir(*ret.response, fileName, req.getURI());
		return (ret);
	}
	handleGetFile(*ret.response, fileName);
	return (ret);
}

responseCgi Location::handlePost(Request &req, char **envp, Server *serv) {
	std::string fileName = getFileName(req.getURI());
	responseCgi ret;
	std::cout << "POST : " << fileName << std::endl;
	req.print();
	if (getCgiExt(req.getURI()).length() != 0)
		return (handleCgi(req, envp, serv));
	ret.isResponse = true;
	ret.response = new Response;
	if (!_allowUpload) {
		ret.response->setCode(403);
		return (ret);
	}
	std::string length = req.getHeader("content-length");
	if (length.length() == 0 || atoi(length.c_str()) == 0) {
		ret.response->setCode(411);
		return (ret);
	}
	std::string uploadFileName = _uploadLocation + "/" + req.getURI();
	std::ofstream ofs(uploadFileName.c_str());
	if (!ofs.is_open() || ofs.fail()) {
		ret.response->setCode(403);
		return (ret);
	}
	ofs.write(req.getContent(), req.getContentSize());
	ofs.close();
	ret.response->setCode(200);
	return (ret);
}

responseCgi Location::handleDelete(Request &req) {
	std::string fileName = getFileName(req.getURI());
	std::cout << "DELETE : " << fileName << std::endl;
	responseCgi ret;
	ret.isResponse = true;
	ret.response = new Response;
	struct stat fileStat;
	if (stat(fileName.c_str(), &fileStat) < 0) {
		ret.response->setCode(404);
		return (ret);
	}
	if (unlink(fileName.c_str()) < 0) {
		ret.response->setCode(403);
		return (ret);
	}
	ret.response->setCode(200);
	return (ret);
}

responseCgi Location::handleRequest(Request &req, char**envp, Server *serv) {
	if (std::find(_allowMeth.begin(), _allowMeth.end(), req.getMethod()) == _allowMeth.end()) {
		responseCgi ret;
		ret.isResponse = true;
		ret.response = new Response(405);
		return (ret);
	}
	if (req.getMethod() == "GET")
		return (handleGet(req, envp, serv));
	if (req.getMethod() == "POST")
		return (handlePost(req, envp, serv));
	return (handleDelete(req));
}

std::string Location::getFileName(std::string &uri) {
	return (_root + "/" + uri.substr(_name.length()));
}

void Location::setServAddr(VirtualServer *addr) {
	_servAddr = addr;
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
