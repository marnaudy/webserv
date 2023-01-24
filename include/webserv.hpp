#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>

typedef struct addressInfo {
	unsigned int maxBodySize;
	u_int32_t address;
} addressInfo;

std::vector<std::string> split(std::string str, char c);
std::string toUpper(std::string str);
std::string toLower(std::string str);
std::string addressItoP(u_int32_t addr);

extern bool g_running;
extern bool g_parent;

#endif
