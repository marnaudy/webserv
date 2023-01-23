#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Buffer.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>

class Bidon {
public:
	virtual ~Bidon() {};
};

class Socket;
class VirtualServer;

class CgiHandler : public Bidon {
public:
	CgiHandler() {}
	virtual ~CgiHandler() {}
	int getFdIn();
	int getFdOut();
	void setSockAddr(Socket *addr);
	void setServAddr(VirtualServer *addr);
	int getPid();
	std::string getScriptName();
	void setScriptPath(std::string path);
	void setBin(std::string &bin);
	void addBody(char *body, size_t size);
	void parseUri(std::string &uri, std::string &ext);
	int checkCgi();
	void exec(Request &req, char **envp);
	void updateEpoll(int epfd);
	void writeToCgi(int epfd);
	void readFromCgi(int epfd);
	void sendCgiResponse(int epfd);
	void closeCgi(int epfd);
private:
	void importEnv(char **env);
	char **exportEnv();
	void addHeadersToEnv(Request &req);
	std::string _scriptName;
	std::string _scriptPath;
	std::vector<std::string> _env;
	int _fdIn;
	int _fdOut;
	Buffer _bufferIn;
	Buffer _bufferOut;
	int _pid;
	Socket *_sockAddr;
	VirtualServer *_servAddr;
	std::string _queryString;
	std::string _extraPath;
	std::string _bin;
};

class CgiException : public std::exception {
public:
	CgiException(std::string message);
	virtual ~CgiException() throw() {}
	virtual const char *what() const throw();
private:
	std::string _message;
};

#endif
