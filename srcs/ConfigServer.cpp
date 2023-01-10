#include "ConfigServer.hpp"

ConfigServer::ConfigServer() : _port(80), _address("0.0.0.0"), _maxBodySize(2000000) {}

bool ConfigServer::isValid() {
	for (std::list<Location>::iterator it = _locations.begin(); it != _locations.end(); ++it) {
		if (!it->isValid())
			return (false);
	}
	return (_locations.size() != 0);
}
