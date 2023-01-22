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

class CgiHandler : public Bidon {
public:
	CgiHandler() {}
	virtual ~CgiHandler() {}
	int exec(Request &req, std::string bin, char **envp);
	int getFdIn();
	int getFdOut();
	void setSockAddr(Socket *addr);
private:
	void importEnv(char **env);
	char **exportEnv();
	void addHeadersToEnv(Request &req);
	std::string _scriptName;
	std::vector<std::string> _env;
	int _fdIn;
	int _fdOut;
	Buffer _bufferIn;
	Buffer _bufferOut;
	int _pid;
	Socket *_sockAddr;
	std::string _queryString;
};

#endif
