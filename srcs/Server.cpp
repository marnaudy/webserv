#include "Server.hpp"

Server::Server() : _config(Config("webserv.conf")) {
	_epfd = epoll_create(10);
	if (_epfd < 0)
		throw std::exception(); //Faire une vraie exception
}

Server::Server(std::string fileName) :_config(Config(fileName)) {
	_epfd = epoll_create(0);
	if (_epfd < 0)
		throw std::exception(); //Faire une vraie exception
}

void Server::printConfig() {
	_config.print();
}

void Server::openSockets() {
	std::map<unsigned int, u_int32_t> portList = _config.getPortList();
	for (std::map<unsigned int, u_int32_t>::iterator it = portList.begin(); it != portList.end(); ++it) {
		Socket newSocket(it->first, it->second, 0);
		newSocket.open(_epfd);
		_sockets.push_back(newSocket);
		newSocket.setFd(-1);
	}
}

void Server::run() {
	openSockets();
	epoll_event evs[MAX_EVENTS];
	while (1) {
		int nbEvents = epoll_wait(_epfd, evs, MAX_EVENTS, 2000);
		std::cout << "Processing events : " << nbEvents << std::endl;
		if (nbEvents < 0)
			throw std::exception(); //Faire une vraie exception
		for (int i = 0; i < nbEvents; ++i) {
			int fd = evs[i].data.fd;
			for (std::list<Socket>::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
				if (it->getFd() == fd) {
					if (it->isClientSocket() && evs[i].events & EPOLLIN)
						it->read(_epfd);
					if (it->isClientSocket() && evs[i].events & EPOLLOUT)
						it->write(_epfd);
					if (!it->isClientSocket() && evs[i].events & EPOLLIN)
						it->acceptConnection(_sockets, _epfd);
				}
			}
		}
	}
}
