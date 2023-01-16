#include "Socket.hpp"

Socket::Socket(unsigned int port, u_int32_t address, bool isClientSocket) :
	_port(port), _address(address), _fd(-1), _isClientSocket(isClientSocket), _closeAfterWrite(false) {}

Socket::Socket(unsigned int port, u_int32_t address, bool isClientSocket, int fd) :
	_port(port), _address(address), _fd(fd), _isClientSocket(isClientSocket), _closeAfterWrite(false) {}

Socket::Socket(const Socket &other) :
	_port(other._port), _address(other._address), _fd(other._fd),
	_isClientSocket(other._isClientSocket), _closeAfterWrite(other._closeAfterWrite) {}

Socket::~Socket() {}

int Socket::getFd() {
	return (_fd);
}

void Socket::setFd(int fd) {
	_fd = fd;
}

bool Socket::isClientSocket() {
	return (_isClientSocket);
}

void Socket::acceptConnection(std::list<Socket> &vec, int epfd) {
	std::cout << "Accepting" << std::endl;
	int newFd = accept(_fd, NULL, 0);
	if (newFd < 0)
		throw SocketException("Error while accepting");
	if (fcntl(newFd, F_SETFL, O_NONBLOCK) < 0)
		throw SocketException("Error setting fd flags");
	Socket newSocket(_port, _address, true, newFd);
	std::list<Socket>::iterator it = vec.insert(vec.end(), newSocket);
	epoll_event ev;
	ev.data.ptr = &*it;
	ev.events = EPOLLIN | EPOLLRDHUP;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, newFd, &ev) < 0)
		throw SocketException("Error epoll add");
}

void Socket::openSocket() {
	_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
		throw SocketException("Error open socket");
	int optVal = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0)
		throw SocketException("Error setting socket flags");
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
		throw SocketException("Error setting fd flags");
	struct sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(_port);
	sockAddr.sin_addr.s_addr = htonl(_address);
	if (bind(_fd, (struct sockaddr *)(&sockAddr), sizeof(sockAddr)) < 0) {
		std::cerr << "Can't bind to port " << _port << " and address "
			<< std::hex << _address << std::dec << std::endl;
		throw SocketException("Error bind");
	}
	if (listen(_fd, MAX_QUEUE) < 0)
		throw SocketException("Error listen");
}

void Socket::readSocket(int epfd) {
	std::cout << "READING" << std::endl;
	char buffer[READ_SIZE];
	ssize_t buffSize = recv(_fd, buffer, READ_SIZE, 0);
	if (buffSize < 0)
		throw SocketException("Error reading");
	_readBuffer.addToBuffer(buffer, static_cast<size_t>(buffSize));


	Request req;
	int parse_len;
	parse_len = req.parse(_readBuffer);
	std::cout << "parse_len = " << parse_len << std::endl;
	while (parse_len != 0) {
		if (parse_len < 0) {
			//Generate bad request response
			Response res(400);
			res.addHeader("bad", "request");
			res.addHeader("tu", "pues");
			char *resBuffer;
			size_t resSize = res.exprt(&resBuffer);
			_writeBuffer.addToBuffer(resBuffer, resSize);
			delete[] resBuffer;
			_closeAfterWrite = true;
			std::cout << "set close after write" << std::endl;
			epoll_event ev;
			ev.data.ptr = this;
			ev.events = EPOLLOUT | EPOLLIN | EPOLLRDHUP;
			if (epoll_ctl(epfd, EPOLL_CTL_MOD, _fd, &ev) < 0)
				throw SocketException("Error epoll mod");
			return;
		}
		_readBuffer.erase(parse_len);
		req.print();
		//Send request to config -> virtualServer
		//Get response
		Response res(200);
		res.addHeader("blob", "plouf");
		char *resBuffer;
		size_t resSize = res.exprt(&resBuffer);
		_writeBuffer.addToBuffer(resBuffer, resSize);
		delete[] resBuffer;

		epoll_event ev;
		ev.data.ptr = this;
		ev.events = EPOLLOUT | EPOLLIN | EPOLLRDHUP;
		if (epoll_ctl(epfd, EPOLL_CTL_MOD, _fd, &ev) < 0)
			throw SocketException("Error epoll mod");
		req = Request();
		parse_len = req.parse(_readBuffer);
	std::cout << "parse_len = " << parse_len << std::endl;
	}
}

int Socket::writeSocket(int epfd) {
	std::cout << "Writing" << std::endl;
	ssize_t lenSent = send(_fd, _writeBuffer.getContent(), _writeBuffer.getSize(), 0);
	if (lenSent < 0)
		throw SocketException("Error writing");
	_writeBuffer.erase(static_cast<size_t>(lenSent));
	if (_writeBuffer.getSize() == 0) {
		epoll_event ev;
		ev.data.ptr = this;
		ev.events = EPOLLIN | EPOLLRDHUP;
		if (epoll_ctl(epfd, EPOLL_CTL_MOD, _fd, &ev) < 0)
			throw SocketException("Error epoll mod");
		if (_closeAfterWrite)
			return (1);
	}
	return (0);
}

void Socket::closeSocket(int epfd) {
	std::cout << "Closing socket" << std::endl;
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, _fd, NULL) < 0) {
		throw SocketException("Error epoll del");
	}
	close(_fd);
}

SocketException::SocketException(std::string message) : _message(message) {}

const char *SocketException::what() const throw() {
	return (_message.c_str());
}
