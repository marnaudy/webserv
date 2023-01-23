#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <cstdlib>
#include <vector>
#include <string>

typedef struct addressInfo {
	unsigned int maxBodySize;
	u_int32_t address;
} addressInfo;

std::vector<std::string> split(std::string str, char c);
std::string toUpper(std::string str);
std::string toLower(std::string str);

extern bool g_running;

#endif
