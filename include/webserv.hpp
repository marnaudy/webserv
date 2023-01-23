#ifndef WEBSER_HPP
#define WEBSER_HPP

#include <cstdlib>
#include <vector>
#include <string>

typedef struct addressInfo {
	unsigned int maxBodySize;
	u_int32_t address;
} addressInfo;

std::vector<std::string> split(std::string str, char c);

bool g_running = true;

#endif
