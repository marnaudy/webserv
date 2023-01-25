#include "Server.hpp"

void Server::loadServer(std::string fileName) {
	_config.loadConfig(fileName);
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

void Server::dealSocketEvent(Socket *sock, u_int32_t event, char **envp) {
	if (event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
		closeSocket(*sock);
		return;
	}
	if (sock->isClientSocket() && event & EPOLLOUT) {
		if (sock->writeSocket(_epfd) != 0) {
			closeSocket(*sock);
			return;
		}
	}
	if (sock->isClientSocket() && event & EPOLLIN)
		sock->readSocket(_epfd, _config, envp, this);
	if (!sock->isClientSocket() && event & EPOLLIN)
		sock->acceptConnection(_sockets, _epfd);
}

int Server::dealCgiEvent(CgiHandler *cgi, u_int32_t event) {
	// if (event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
	if (event & (EPOLLOUT | EPOLLHUP | EPOLLRDHUP)) {
		cgi->writeToCgi(_epfd);
	}
	if (event & (EPOLLIN | EPOLLHUP | EPOLLRDHUP)) {
		cgi->readFromCgi(_epfd);
	}
	if (event & (EPOLLERR)) {
		cgi->closeCgi(_epfd, true);
		return (1);
	}
	return (0);
}

void Server::run(char **envp) {
	openSockets();
	epoll_event evs[MAX_EVENTS];
	while (g_running) {
		int nbEvents = epoll_wait(_epfd, evs, MAX_EVENTS, -1);
		if (nbEvents < 0) {
			if (errno != EINTR)
				throw SocketException("Error epoll wait");
		}
		for (int i = 0; i < nbEvents; ++i) {
			Bidon *bid = reinterpret_cast<Bidon *>(evs[i].data.ptr);
			Socket *sock = dynamic_cast<Socket *>(bid);
			CgiHandler *cgi = dynamic_cast<CgiHandler *>(bid);
			if (sock != NULL)
				dealSocketEvent(sock, evs[i].events, envp);
			else {
				if (dealCgiEvent(cgi, evs[i].events) != 0)
					break;
			}
		}
	}
}

void Server::closeServer() {
	for (std::list<Socket>::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
		it->closeSocket(_epfd);
	}
	close(_epfd);
}

void Server::closeFds() {
	close(_epfd);
	for (std::list<Socket>::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
		it->closeSocketFds();
	}
}
