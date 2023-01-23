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

void CgiHandler::setServAddr(VirtualServer *addr) {
	_servAddr = addr;
}

int CgiHandler::getPid() {
	return (_pid);
}

std::string CgiHandler::getScriptName() {
	return (_scriptName);
}

void CgiHandler::setScriptPath(std::string path) {
	_scriptPath = path;
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

void CgiHandler::parseUri(std::string &uri, std::string &ext) {
	size_t extPos = uri.find(ext + "/");
	size_t extPos2 = uri.find(ext + "?");
	if (extPos < extPos2) {
		_scriptName = uri.substr(0, extPos + ext.size());
		_extraPath = uri.substr(extPos + ext.size());
	} 
	else if (extPos2 < extPos) {
		_scriptName = uri.substr(0, extPos2 + ext.size());
		_extraPath = uri.substr(extPos2 + ext.size());
		_queryString = _extraPath.substr(_extraPath.find("?") + 1);
		_extraPath = _extraPath.substr(0, _extraPath.find("?"));
	} else {
		_scriptName = uri;
	}
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
    _env.push_back("PATH_INFO = " + _extraPath);
    _env.push_back("PATH_TRANSLATED = " + _extraPath);
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
		_env.push_back("HTTP_" + toUpper(it->first) + " = " + it->second);
	}
}

void CgiHandler::exec(Request &req, std::string bin, char **envp) {
	importEnv(envp);
    addHeadersToEnv(req);
	envp = exportEnv();
	int pipeIn[2], pipeOut[2];
	if (pipe(pipeIn) == -1) {
		throw CgiException("couldn't open pipe");
	}
	if (pipe(pipeOut) == -1) {
		close(pipeIn[0]);
		close(pipeIn[1]);
		throw CgiException("couldn't open pipe");
	}
	_fdIn = pipeIn[1];
	_fdOut = pipeOut[0];
	_pid = fork();
	if (_pid < 0) {
		_pid = 0;
		close(pipeIn[0]);
		close(pipeIn[1]);
		close(pipeOut[0]);
		close(pipeOut[1]);
		throw CgiException("couldn't fork");
	}
	if (_pid == 0) {
		_pid = -1;
		close(pipeIn[1]);
		close(pipeOut[0]);
		if (dup2(pipeIn[0], STDIN_FILENO) < 0) {
			close(pipeIn[0]);
			close(pipeOut[1]);
			throw CgiException("couldn't dup2");
		}
		if (dup2(pipeOut[1], STDOUT_FILENO) < 0) {
			close(pipeIn[0]);
			close(pipeOut[1]);
			throw CgiException("couldn't dup2");
		}
		close(pipeIn[0]);
		close(pipeOut[1]);
		std::string scriptDir = _scriptPath.substr(0, _scriptPath.find_last_of("/"));
		if (chdir(scriptDir.c_str()) == -1) {
			throw CgiException("couldn't chdir");
		}
		char *argv[] = {strdup(bin.c_str()), NULL};
		if (argv[0] == NULL)
			throw CgiException("couldn't strdup");
		execve(bin.c_str(), argv, envp);
		delete argv[0];
		throw CgiException("couldn't execve");
	}
	close(pipeIn[0]);
	close(pipeOut[1]);
}

CgiException::CgiException(std::string message) : _message(message) {}

const char *CgiException::what() const throw() {
	return (_message.c_str());
}
