#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Buffer.hpp"
#include "Response.hpp"
#include <cstdlib>
#include <sys/socket.h>

class Socket {
public:
	Socket() {}
	Socket(unsigned int port, u_int32_t address);
	~Socket() {}
	void read();
	void write();
private:
	unsigned int _port;
	u_int32_t _address;
	int _fd;
	Buffer _readBuffer;
	Buffer _writeBuffer;
};

#endif
