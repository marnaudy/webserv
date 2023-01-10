#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <list>

enum method_e {
	GET,
	POST,
	DELETE
};

class Location {
public:
	Location();
	~Location() {}

	bool isValid();
private:
	std::string _root;
	std::list<enum method_e> _allowMeth;
	bool _autoindex;
	std::string _dirPage;
	bool _allowUpload;
	std::string _uploadLocation;
	//CGI here
};

#endif
