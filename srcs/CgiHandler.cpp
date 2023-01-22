#include "CgiHandler.hpp"

int CgiHandler::getFdIn() {
	return (_fdIn);
}

int CgiHandler::getFdOut() {
	return (_fdOut);
}

void CgiHandler::setSockAddr(Socket *addr) {
	_sockAddr = addr;
}

void CgiHandler::importEnv(char **env) {
	int i = 0;
	while (env && env[i]) {
		_env.push_back(env[i]);
		i++;
	}
}

char **CgiHandler::exportEnv() {
	char **envp;
	envp = new char *[_env.size() + 1];
	for (int i = 0; i < _env.size(); ++i) {
		envp[i] = strdup(_env[i].c_str());
		if (envp[i] == NULL) {
			int j = 0;
			while (j < i) {
				delete envp[j];
				j++;
			}
			delete[] envp;
			throw std::exception();
		}
	}
	envp[_env.size()] = NULL;
	return (envp);
}

std::string toUpper(std::string str) {
	for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
		*it = toupper(*it);
	}
	return (str);
}

void CgiHandler::addHeadersToEnv(Request &req) {
	std::map<std::string, std::string> headers = req.getHeaders();
	if (req.getContentSize() != 0) {
		_env.push_back("CONTENT_LENGTH = " + req.getHeader("content-length"));
		_env.push_back("CONTENT_TYPE = " + req.getHeader("content-type"));
	}
    _env.push_back("GATEWAY_INTERFACE = CGI/1.1");
    _env.push_back("QUERY_STRING = " + _queryString);
    _env.push_back("REQUEST_METHOD = " + req.getMethod());
    _env.push_back("SCRIPT_NAME = " + _scriptName);
    std::ostringstream ss;
    ss << req.getAddress();
    _env.push_back("SERVER_NAME = " + ss.str());
    std::ostringstream sss;
    sss << req.getPort();
    _env.push_back("SERVER_PORT = " + sss.str());
    _env.push_back("SERVER_PROTOCOL = HTTP/1.1");
    _env.push_back("SERVER_SOFTWARE = webserv");
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
		_env.push_back("HTTP_" + toUpper(it->first) + " = " + it->second);
	}
}

int CgiHandler::exec(Request &req, std::string bin, char **envp) {
	importEnv(envp);
    addHeadersToEnv(req);
}
