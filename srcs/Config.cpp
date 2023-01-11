#include "Config.hpp"

bool Config::isValid() {
	for (std::list<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		if (!it->isValid())
			return (false);
	}
	return (_servers.size() != 0);
}

void Config::parse(std::ifstream &ifs) {
	std::string line;
	std::getline(ifs, line);
	while (!ifs.eof()) {
		if (parseDirective(line) == "server") {
			Server serv;
			serv.parse(ifs);
			_servers.push_back(serv);
		} else {
			throw BadConfigException("Unknown directive");
		}
		std::getline(ifs, line);
	}
}

void Config::print() {
	for (std::list<Server>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		std::cout << "Server " << std::endl;
		it->print();
	}
}
