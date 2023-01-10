#include "Location.hpp"

Location::Location() : _allowMeth(GET), _autoindex(true), _allowUpload(false) {}

bool Location::isValid() {
	return ( _root.length() != 0
		&& _allowMeth.size() != 0
		&& (_allowUpload || _uploadLocation.length() != 0));
}
