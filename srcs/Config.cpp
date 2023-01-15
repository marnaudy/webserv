#include "Config.hpp"

Config::Config(std::string fileName) {
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

void Config::parse(std::ifstream &ifs) {
	std::string line;
	std::getline(ifs, line);
	while (!ifs.eof()) {
		if (parseDirective(line) == "server") {
			VirtualServer serv;
			serv.parse(ifs);
			_servers.push_back(serv);
		} else {
			throw BadConfigException("Unknown directive");
		}
		std::getline(ifs, line);
	}
	if (_servers.size() == 0)
		throw BadConfigException("No server");
}

void Config::print() {
	for (std::list<VirtualServer>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		std::cout << "VirtualServer " << std::endl;
		it->print();
	}
}

std::map<unsigned int,u_int32_t> Config::getPortList() {
	std::map<unsigned int,u_int32_t> portList;
	for (std::list<VirtualServer>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		if (portList.find(it->getPort()) == portList.end())
			portList.insert(std::pair<unsigned int, u_int32_t>(it->getPort(), it->getAddress()));
		else {
			portList.erase(it->getPort());
			portList.insert(std::pair<unsigned int, u_int32_t>(it->getPort(), 0));
		}
	}
	return (portList);
}
