#include "CgiHandler.hpp"
#include "Socket.hpp"
#include "Server.hpp"

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

void CgiHandler::setBin(std::string &bin) {
	_bin = bin;
}

void CgiHandler::addBody(char *body, size_t size) {
	_bufferIn.addToBuffer(body, size);
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
	for (unsigned int i = 0; i < _env.size(); ++i) {
		envp[i] = &_env[i][0];
	}
	envp[_env.size()] = NULL;
	return (envp);
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

int CgiHandler::checkCgi() {
	struct stat fileStat;
	if (stat(_scriptPath.c_str(), &fileStat) < 0) {
		return (404);
	}
	if (!(fileStat.st_mode & S_IRUSR)) {
		return (403);
	}
	if (stat(_bin.c_str(), &fileStat) < 0) {
		return (404);
	}
	if (!(fileStat.st_mode & S_IXUSR)) {
		return (403);
	}
	return (0);
}

void CgiHandler::addHeadersToEnv(Request &req) {
	std::map<std::string, std::string> headers = req.getHeaders();
	if (req.getContentSize() != 0) {
		_env.push_back("CONTENT_LENGTH=" + req.getHeader("content-length"));
		_env.push_back("CONTENT_TYPE=" + req.getHeader("content-type"));
	}
	_env.push_back("REDIRECT_STATUS=200");
    _env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    _env.push_back("QUERY_STRING=" + _queryString);
    _env.push_back("REQUEST_METHOD=" + req.getMethod());
    _env.push_back("SCRIPT_NAME=" + _scriptName);
    _env.push_back("SCRIPT_FILENAME=" + _scriptPath.substr(_scriptPath.find_last_of("/") + 1));
    _env.push_back("SERVER_NAME=" + addressItoP(req.getAddress()));
    std::ostringstream sss;
    sss << req.getPort();
    _env.push_back("SERVER_PORT=" + sss.str());
    _env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    _env.push_back("SERVER_SOFTWARE=webserv");
    if (_extraPath.length() != 0)
		_env.push_back("PATH_INFO=" + _extraPath);
	else
    	_env.push_back("PATH_INFO=/");
	if (_extraPath.length() != 0)
		_env.push_back("PATH_TRANSLATED=" + _extraPath);
	else
    	_env.push_back("PATH_TRANSLATED=/");
	for (std::map<std::string, std::string>::iterator it=headers.begin(); it != headers.end(); ++it) {
		_env.push_back("HTTP_" + toUpper(it->first) + "=" + it->second);
	}
}

std::string updateBin(std::string &bin, std::string &scriptPath) {
	if (bin[0] == '/')
		return (bin);
	int subdirCount = 0;
	std::vector<std::string> splitPath = split(scriptPath, '/');
	for (unsigned int i = 0; i < splitPath.size(); ++i) {
		if (splitPath[i] == "..")
			subdirCount--;
		else if (splitPath[i] != ".")
			subdirCount++;
	}
	std::string newBin;
	for (int i = 0; i < subdirCount; ++i) {
		newBin += "../";
	}
	newBin += bin;
	return (newBin);
}

void CgiHandler::exec(Request &req, char **envp, Server *serv) {
	importEnv(envp);
    addHeadersToEnv(req);
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
		g_parent = false;
		serv->closeFds();
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
		std::string scriptName = _scriptPath.substr(_scriptPath.find_last_of("/") + 1);
		if (chdir(scriptDir.c_str()) == -1) {
			throw CgiException("couldn't chdir");
		}
		std::string binUpdated = updateBin(_bin, scriptDir);
		char **envpNew = exportEnv();
		char *argv[] = {&binUpdated[0], &scriptName[0], NULL};
		execve(binUpdated.c_str(), argv, envpNew);
		delete[] envpNew;
		throw CgiException("couldn't execve");
	}
	close(pipeIn[0]);
	close(pipeOut[1]);
	fcntl(_fdIn, F_SETFL, O_NONBLOCK);
	fcntl(_fdOut, F_SETFL, O_NONBLOCK);
}

void CgiHandler::updateEpoll(int epfd) {
	epoll_event ev;
	ev.data.ptr = this;
	ev.events = EPOLLOUT | EPOLLRDHUP;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, _fdIn, &ev) == -1) {
		throw CgiException("Epoll add");
	}
	ev.events = EPOLLIN | EPOLLRDHUP;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, _fdOut, &ev) == -1) {
		throw CgiException("Epoll add");
	}
}

void CgiHandler::writeToCgi(int epfd) {
	if (_fdIn < 0)
		return;
	if (_bufferIn.getSize() == 0) {
		if (epoll_ctl(epfd, EPOLL_CTL_DEL, _fdIn, NULL) < 0)
			throw CgiException("Error epoll del cgi pipe");
		close(_fdIn);
		_fdIn = -1;
		return;
	}
	int ret = write(_fdIn, _bufferIn.getContent(), _bufferIn.getSize());
	if (ret == -1) {
		throw CgiException("write to cgi failed");
	}
	_bufferIn.erase(ret);
}

void CgiHandler::readFromCgi(int epfd) {
	char buf[READ_SIZE];
	int ret = read(_fdOut, buf, READ_SIZE);
	if (ret == -1) {
		throw CgiException("read from cgi failed");
	}
	if (ret == 0) {
		if (epoll_ctl(epfd, EPOLL_CTL_DEL, _fdOut, NULL) < 0)
			throw CgiException("Error epoll del cgi pipe");
		close(_fdOut);
		_fdOut = -1;
		if (_fdIn >= 0) {
			if (epoll_ctl(epfd, EPOLL_CTL_DEL, _fdIn, NULL) < 0)
				throw CgiException("Error epoll del cgi pipe");
			close(_fdIn);
			_fdIn = -1;
		}
		sendCgiResponse(epfd);
		closeCgi(epfd, true);
		return;
	}
	_bufferOut.addToBuffer(buf, ret);
}

void CgiHandler::sendCgiResponse(int epfd) {
	Response *res = new Response(200);
	int status;
	if (_bufferOut.getSize() == 0) {
		std::cout << "CGI script didn't send anyting" << std::endl;
		res->setCode(500);
	} else {
		std::string line = _bufferOut.getLine2(status);
		while (status == 0 && line.length() != 0) {
			std::string field = toLower(line.substr(0, line.find(":")));
			if (field == "status") {
				res->setCode(atoi(line.substr(line.find(" ") + 1).c_str()));
			} else if (field != "content-length") {
				res->addHeaderLine(line);
			}
			line = _bufferOut.getLine2(status);
		}
		if (status == 0) {
			_bufferOut.erase(_bufferOut.getPos());
			res->setContent(_bufferOut.getContent(), _bufferOut.getSize());
		} else {
			res->setContent(NULL, 0);
		}
	}
	if (res->getCode() >= 300 && res->getCode() < 600)
		_servAddr->handleError(*res);
	char *resBuffer;
	size_t resSize = res->exprt(&resBuffer);
	Buffer *sockBuff = _sockAddr->getWriteBuffer();
	sockBuff->addToBuffer(resBuffer, resSize);
	delete[] resBuffer;
	delete res;
	epoll_event ev;
	ev.data.ptr = _sockAddr;
	ev.events = EPOLLOUT | EPOLLIN | EPOLLRDHUP;
	if (epoll_ctl(epfd, EPOLL_CTL_MOD, _sockAddr->getFd(), &ev) < 0)
		throw CgiException("Error epoll mod");
}

void CgiHandler::closeCgi(int epfd, bool removeFromSocket) {
	if (g_parent && _fdIn >= 0) {
		if (epoll_ctl(epfd, EPOLL_CTL_DEL, _fdIn, NULL) < 0)
			throw CgiException("Error epoll delete");
		close(_fdIn);
	}
	if (g_parent && _fdOut >= 0) {
		if (epoll_ctl(epfd, EPOLL_CTL_DEL, _fdOut, NULL) < 0)
			throw CgiException("Error epoll delete");
		close(_fdOut);
	}
	if (g_parent && _pid > 0) {
		kill(_pid, SIGTERM);
		waitpid(_pid, NULL, 0);
	}
	if (removeFromSocket)
		_sockAddr->removeCgi(this);
}

CgiException::CgiException(std::string message) : _message(message) {}

const char *CgiException::what() const throw() {
	return (_message.c_str());
}
