#include "Socket.hpp"

void Socket::read() {
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
}

void Socket::write() {
	ssize_t lenSent = send(_fd, _writeBuffer.getContent(), _writeBuffer.getSize());
	if (lenSent < 0)
		throw std::exception(); //Custom exception here
	_writeBuffer.erase(static_cast<size_t>(lenSent));
}
