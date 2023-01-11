#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <cstdlib>

class Location {
public:
	Location(std::string &name);
	~Location() {}
	bool isValid();
	void parse(std::ifstream &ifs);
	void print();
private:
	std::string _name;
	std::string _root;
	std::list<std::string> _allowMeth;
	bool _autoindex;
	std::string _dirPage;
	bool _allowUpload;
	std::string _uploadLocation;
	//CGI here
	void parseMeth(std::string value);
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
