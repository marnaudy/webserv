#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Buffer.hpp"
#include "Response.hpp"
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <list>
#include "fcntl.h"

#define MAX_QUEUE 10

class Socket {
public:
	Socket(unsigned int port, u_int32_t address, bool isClientSocket);
	Socket(unsigned int port, u_int32_t address, bool isClientSocket, int fd);
	Socket(const Socket &other);
	~Socket();
	bool operator==(const Socket &rhs);
	int getFd();
	void setFd(int fd);
	bool isClientSocket();
	void acceptConnection(std::list<Socket> &vec, int epfd);
	void openSocket();
	void readSocket(int epfd);
	void writeSocket(int epfd);
	void closeSocket(int epfd);
private:
	unsigned int _port;
	u_int32_t _address;
	int _fd;
	Buffer _readBuffer;
	Buffer _writeBuffer;
	bool _isClientSocket;
};

class SocketException : public std::exception {
public:
	SocketException(std::string message);
	virtual ~SocketException() throw() {}
	virtual const char *what() const throw();
private:
	std::string _message;
};

#endif
