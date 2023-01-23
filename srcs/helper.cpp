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

