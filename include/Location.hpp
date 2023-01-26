#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <map>
#include <list>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include "Request.hpp"
#include "Response.hpp"
#include "CgiHandler.hpp"

struct responseCgi {
	bool isResponse;
	Response *response;
	CgiHandler *cgi;
};

class Location {
public:
	Location(std::string name);
	~Location() {}
	void checkConfig();
	void parse(std::ifstream &ifs);
	void print();
	unsigned int match(std::string &uri);
	void handleDirList(Response &res, std::string dirName, std::string &uri);
	void handleGetFile(Response &res, std::string fileName);
	void handleGetDir(Response &res, std::string fileName, std::string &uri);
	responseCgi handleCgi(Request &req, char** envp, Server *serv);
	responseCgi handleGet(Request &req, char** envp, Server *serv);
	responseCgi handlePost(Request &req, char** envp, Server *serv);
	responseCgi handleDelete(Request &req, char **envp, Server *serv);
	responseCgi handleRequest(Request &req, char**envp, Server *serv);
	std::string getFileName(std::string &uri);
	void setServAddr(VirtualServer *servAddr);
private:
	void parseMeth(std::string value);
	void parseCGI(std::string value);
	std::string getCgiExt(std::string &uri);

	std::string _name;
	std::string _root;
	std::list<std::string> _allowMeth;
	bool _autoindex;
	std::string _dirPage;
	bool _allowUpload;
	std::string _uploadLocation;
	std::map<std::string, std::string> _cgi;
	unsigned int _returnCode;
	std::string _returnDest;
	VirtualServer *_servAddr;
};

class BadConfigException : public std::exception {
public:
	BadConfigException(std::string message);
	virtual ~BadConfigException() throw() {}
	virtual const char *what() const throw();
private:
	std::string _message;
};

std::string parseDirective(std::string &line);
std::string parseValue(std::string &line);

#endif
