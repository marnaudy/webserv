#include "Config.hpp"

void Config::loadConfig(std::string fileName) {
	std::ifstream ifs(fileName.c_str());
	if (!ifs.is_open())
		throw BadConfigException("Can't open confid file");
	try {
		parse(ifs);
	} catch (BadConfigException &e) {
		ifs.close();
		throw BadConfigException(e.what());
	}
	ifs.close();
}

void Config::print() {
	for (std::list<VirtualServer>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		std::cout << "VirtualServer " << std::endl;
		it->print();
	}
}

std::map<unsigned int,std::list<addressInfo> > Config::getPortList() {
	std::map<unsigned int,std::list<addressInfo> > portList;
	for (std::list<VirtualServer>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		std::list<addressInfo> &list = portList[it->getPort()];
		addressInfo addr;
		addr.address = it->getAddress();
		addr.maxBodySize = it->getMaxBodySize();
		list.push_back(addr);
	}
	return (portList);
}

responseCgi Config::handleRequest(Request &req, char **envp, Server *serv) {
	unsigned int port = req.getPort();
	u_int32_t addr = req.getAddress();
	std::string host = req.getHeader("host");
	VirtualServer *chosenServer = NULL;
	for (std::list<VirtualServer>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		if (it->getPort() == port && (it->getAddress() == addr || it->getAddress() == 0)) {
			if (chosenServer == NULL)
				chosenServer = &*it;
			if (it->getAddress() != 0 && chosenServer->getAddress() == 0)
				chosenServer = &*it;
			if (it->matchHost(host) && !chosenServer->matchHost(host))
				chosenServer = &*it;
		}
	}
	if (chosenServer == NULL)
		throw BadConfigException("Request doesn't match any server");
	return (chosenServer->handleRequest(req, envp, serv));
}

void Config::parse(std::ifstream &ifs) {
	std::string line;
	std::getline(ifs, line);
	while (!ifs.eof()) {
		if (parseDirective(line) == "server") {
			VirtualServer serv;
			std::list<VirtualServer>::iterator it = _servers.insert(_servers.end(), serv);
			it->parse(ifs);
		} else {
			throw BadConfigException("Unknown directive");
		}
		std::getline(ifs, line);
	}
	if (_servers.size() == 0)
		throw BadConfigException("No server");
}
