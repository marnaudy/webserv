#include "Config.hpp"

bool Config::isValid() {
	for (std::list<ConfigServer>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		if (!it->isValid())
			return (false);
	}
	return (_servers.size() != 0);
}
