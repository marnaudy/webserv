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
		sock->readSocket(_epfd, _config, envp);
	if (!sock->isClientSocket() && event & EPOLLIN)
		sock->acceptConnection(_sockets, _epfd);
}

void Server::dealCgiEvent(CgiHandler *cgi, u_int32_t event) {
	(void) cgi;
	if (event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
		//Close cgi handler
		return;
	}
	if (event & EPOLLOUT) {
		//Write to CGI
		return;
	}
	if (event & EPOLLIN)
		//Read from CGI
		return;
}

void Server::run(char **envp) {
	openSockets();
	epoll_event evs[MAX_EVENTS];
	while (g_running) {
		int nbEvents = epoll_wait(_epfd, evs, MAX_EVENTS, -1);
		std::cout << "Processing events : " << nbEvents << std::endl;
		if (nbEvents < 0)
			throw SocketException("Error epoll wait");
		for (int i = 0; i < nbEvents; ++i) {
			Bidon *bid = reinterpret_cast<Bidon *>(evs[i].data.ptr);
			Socket *sock = dynamic_cast<Socket *>(bid);
			CgiHandler *cgi = dynamic_cast<CgiHandler *>(bid);
			if (sock != NULL)
				dealSocketEvent(sock, evs[i].events, envp);
			else
				dealCgiEvent(cgi, evs[i].events);
		}
	}
}

void Server::closeServer() {
	for (std::list<Socket>::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
		it->closeSocket(_epfd);
	}
	close(_epfd);
}

