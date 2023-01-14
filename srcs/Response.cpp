#include "Response.hpp"

Response::Response() {}

Response::Response(unsigned int code) {
	setCode(code);
}

Response::~Response() {}

void Response::setCode(unsigned int code) {
	_code = code;
	switch (code) {
	case 200:
		_message = "OK";
		break;
	case 201:
		_message = "Created";
		break;
	case 301:
		_message = "Moved Permanently";
		break;
	case 302:
		_message = "Found";
		break;
	case 303:
		_message = "See other";
		break;
	case 304:
		_message = "Not Modified";
		break;
	case 305:
		_message = "Use Proxy";
		break;
	case 307:
		_message = "Temporary Redirect";
		break;
	case 308:
		_message = "Permanent Redirect";
		break;
	case 400:
		_message = "Bad Request";
		break;
	case 403:
		_message = "Forbidden";
		break;
	case 404:
		_message = "Not Found";
		break;
	case 405:
		_message = "Method Not Allowed";
		break;
	case 411:
		_message = "Length Required";
		break;
	case 413:
		_message = "Payload Too Large";
		break;
	case 414:
		_message = "URI Too Long";
		break;
	case 431:
		_message = "Request Header Fields Too Large";
		break;
	case 501:
		_message = "Not Implemented";
		break;
	case 505:
		_message = "HTTP Version Not Supported";
		break;
	default:
		_message = "";
		break;
	}
}

void Response::addHeader(std::string field, std::string value) {
	if (_headers.find(field) != _headers.end())
		_headers.erase(field);
	_headers.insert(std::pair<std::string, std::string>(field, value));
}

size_t Response::exprt(char **buffer) {
	std::string message;
	std::ostringstream ss;
	ss << _code;
	message = "HTTP/1.1 " + ss.str() + " " + _message + "\r\n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
		message += it->first + ": " + it->second + "\r\n";
	}
	message += "\r\n";
	size_t size = message.length() + _content.size();
	*buffer = new char[size]();
	if (! *buffer)
		return (0); //Throw would be better
	unsigned int i;
	for (i = 0; i < message.length(); ++i) {
		(*buffer)[i] = message[i];
	}
	for (unsigned int j = 0; j < _content.size(); ++j) {
		(*buffer)[i + j] = _content[j];
	}
	return (size);
}
