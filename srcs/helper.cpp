#include "webserv.hpp"

std::vector<std::string> split(std::string str, char c) {
	size_t end = 0;
	size_t start = 0;
	std::vector<std::string> list;
	while (end != std::string::npos) {
		end = str.find_first_of(c, start);
		list.push_back(str.substr(start, end - start));
		start = end + 1;
	}
	return (list);
}

std::string toUpper(std::string str) {
	for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
		*it = toupper(*it);
	}
	return (str);
}

std::string toLower(std::string str) {
	for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
		*it = tolower(*it);
	}
	return (str);
}

std::string addressItoP(u_int32_t addr) {
	std::string str;
	for (int i = 3; i >= 0; i--) {
		int byte = (addr >> (i * 8)) & 255;
		std::ostringstream ss;
		ss << byte;
		str += ss.str();
		if (i != 0)
			str += '.';
	}
	return (str);
}
