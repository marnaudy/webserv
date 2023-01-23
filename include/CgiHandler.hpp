#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Buffer.hpp"
#include "Request.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <sstream>

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
	void exec(Request &req, std::string bin, char **envp);
	int getFdIn();
	int getFdOut();
	void setSockAddr(Socket *addr);
	void setServAddr(VirtualServer *addr);
	int getPid();
	std::string getScriptName();
	void setScriptPath(std::string path);
	void parseUri(std::string &uri, std::string &ext);
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
