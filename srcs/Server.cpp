#include "Server.hpp"

void Server::loadServer(std::string fileName) {
	_config = Config(fileName);
	_epfd = epoll_create(10);
	if (_epfd < 0)
		throw SocketException("Error epoll create");
}

void Server::printConfig() {
	_config.print();
}

void Server::openSockets() {
	std::map<unsigned int, std::list<addressInfo> > portList = _config.getPortList();
	for (std::map<unsigned int, std::list<addressInfo> >::iterator it = portList.begin(); it != portList.end(); ++it) {
		Socket newSocket(it->first, it->second);
		newSocket.openSocket();
		std::list<Socket>::iterator newIt = _sockets.insert(_sockets.end(), newSocket);
		epoll_event ev;
		ev.data.ptr = &*newIt;
		ev.events = EPOLLIN | EPOLLRDHUP;
		if (epoll_ctl(_epfd, EPOLL_CTL_ADD, newSocket.getFd(), &ev) < 0)
			throw SocketException("Error epoll add");
	}
}

void Server::closeSocket(Socket &sock) {
	sock.closeSocket(_epfd);
	for (std::list<Socket>::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
		if (it->getFd() == sock.getFd()) {
			_sockets.erase(it);
			break;
		}
	}
}

void Server::run() {
	openSockets();
	epoll_event evs[MAX_EVENTS];
	while (1) {
		int nbEvents = epoll_wait(_epfd, evs, MAX_EVENTS, -1);
		std::cout << "Processing events : " << nbEvents << std::endl;
		if (nbEvents < 0)
			throw SocketException("Error epoll wait");
		for (int i = 0; i < nbEvents; ++i) {
			Socket *sock = reinterpret_cast<Socket *>(evs[i].data.ptr);
			if (evs[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
				closeSocket(*sock);
				break;
			}
			if (sock->isClientSocket() && evs[i].events & EPOLLOUT) {
				if (sock->writeSocket(_epfd) != 0) {
					closeSocket(*sock);
					break;
				}
			}
			if (sock->isClientSocket() && evs[i].events & EPOLLIN)
				sock->readSocket(_epfd);
			if (!sock->isClientSocket() && evs[i].events & EPOLLIN)
				sock->acceptConnection(_sockets, _epfd);
		}
	}
}

void Server::closeServer() {
	for (std::list<Socket>::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
		it->closeSocket(_epfd);
	}
	close(_epfd);
}

