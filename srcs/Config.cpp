#include "Config.hpp"

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
