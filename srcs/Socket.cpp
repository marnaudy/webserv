#include "Socket.hpp"

Socket::Socket(unsigned int port, u_int32_t address, bool isClientSocket) :
	_port(port), _address(address), _fd(-1), _isClientSocket(isClientSocket) {}

Socket::Socket(unsigned int port, u_int32_t address, bool isClientSocket, int fd) :
	_port(port), _address(address), _fd(fd), _isClientSocket(isClientSocket) {}

Socket::Socket(const Socket &other) :
	_port(other._port), _address(other._address), _fd(other._fd), _isClientSocket(other._isClientSocket) {}

Socket::~Socket() {
	// if (_fd >= 0)
	// 	close(_fd);
}

int Socket::getFd() {
	return (_fd);
}

void Socket::setFd(int fd) {
	_fd = fd;
}

bool Socket::isClientSocket() {
	return (_isClientSocket);
}

void Socket::open(int epfd) {
	_fd = socket(PF_INET, SOCK_STREAM, 0);
	int optVal = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0) {
		std::cout << "setsockopt" << std::endl;
		throw std::exception();//Faire une vraie exception
	}
	if (_fd < 0)
		throw std::exception();//Faire une vraie exception
	struct sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(_port);
	sockAddr.sin_addr.s_addr = htonl(_address);
	if (bind(_fd, (struct sockaddr *)(&sockAddr), sizeof(sockAddr)) < 0) {
		std::cout << "bind" << std::endl;
		throw std::exception();//Faire une vraie exception
	}
	if (listen(_fd, MAX_QUEUE) < 0)
		throw std::exception();//Faire une vraie exception
	epoll_event ev;
	ev.data.fd = _fd;
	ev.events = EPOLLIN;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, _fd, &ev) < 0)
		throw std::exception();//Faire une vraie exception
	char addr[50];
	inet_ntop(AF_INET, &sockAddr.sin_addr.s_addr, addr, 50);
	std::cout << addr << " " << _port << " " << _fd << std::endl;
}

void Socket::read(int epfd) {
	std::cout << "READING" << std::endl;
	char buffer[READ_SIZE];
	ssize_t buffSize = recv(_fd, buffer, READ_SIZE, 0);
	if (buffSize < 0)
		throw std::exception(); //Custom exception here
	_readBuffer.addToBuffer(buffer, static_cast<size_t>(buffSize));
	//Parse read buffer to a request
	Response res(200);
	res.addHeader("blob", "plouf");
	char *resBuffer;
	size_t resSize = res.exprt(&resBuffer);
	_writeBuffer.addToBuffer(resBuffer, resSize);
	epoll_event ev;
	ev.data.fd = _fd;
	ev.events = EPOLLOUT & EPOLLIN;
	if (epoll_ctl(epfd, EPOLL_CTL_MOD, _fd, &ev) < 0)
		throw std::exception(); //Custom exception here
}

void Socket::write(int epfd) {
	std::cout << "Writing" << std::endl;
	ssize_t lenSent = send(_fd, _writeBuffer.getContent(), _writeBuffer.getSize(), 0);
	if (lenSent < 0)
		throw std::exception(); //Custom exception here
	_writeBuffer.erase(static_cast<size_t>(lenSent));
	if (_writeBuffer.getSize() == 0) {
		epoll_event ev;
		ev.data.fd = _fd;
		ev.events = EPOLLIN;
		if (epoll_ctl(epfd, EPOLL_CTL_MOD, _fd, &ev) < 0)
			throw std::exception(); //Custom exception here
	}
}

void Socket::acceptConnection(std::list<Socket> &vec, int epfd) {
	std::cout << "Accepting" << std::endl;
	int newFd = accept(_fd, NULL, 0);
	if (newFd < 0)
		throw std::exception(); //Custom exception here
	Socket newSocket(_port, _address, true, newFd);
	vec.push_back(newSocket);
	epoll_event ev;
	ev.data.fd = newFd;
	ev.events = EPOLLIN;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, newFd, &ev) < 0)
		throw std::exception(); //Custom exception here
}
