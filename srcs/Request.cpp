#include "Request.hpp"

Request::Request(unsigned int port, u_int32_t address) : _port(port), _address(address), _errorCode(0) {}

std::vector<std::string> split(std::string str, char c) {
	size_t end = 0;
	size_t start = 0;
	std::vector<std::string> list;
	while (end != std::string::npos) {
		end = str.find_first_of(c, start);
		list.push_back(str.substr(start, end - start));
		start = end + 1;
	}
	return (list);
}

int Request::parse(Buffer &buf, unsigned int maxBodySize) {
	buf.setPos(0);
	int status;
	status = parseFirstLine(buf);
	if (status < 0)
		return (status);
	status = 0;
	while (status == 0) {
		status = parseHeaderLine(buf);
	}
	if (status < 0)
		return (0);
	return (parseContent(buf, maxBodySize));
}

void Request::print() {
	std::cout << "Request" << std::endl;
	std::cout << "Method = " << _method << std::endl;
	std::cout << "uri = " << _uri << std::endl;
	std::cout << "version = " << _version << std::endl;
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
	write(1, &_content[0], _content.size());
	std::cout << std::endl;
}

unsigned int Request::getPort() {
	return (_port);
}

u_int32_t Request::getAddress() {
	return (_address);
}

std::string &Request::getHeader(std::string field) {
	return (_headers[field]);
}

std::string &Request::getMethod() {
	return (_method);
}

std::string &Request::getURI() {
	return (_uri);
}

std::string &Request::getVersion() {
	return (_version);
}

unsigned int Request::getErrorCode() {
	return (_errorCode);
}

void Request::setErrorCode(unsigned int code) {
	_errorCode = code;
}

char *Request::getContent() {
	return (&_content[0]);
}

unsigned int Request::getContentSize() {
	return (_content.size());
}

std::string parseHeaderField(std::string &line) {
	size_t end = line.find_first_of(" ");
	std::string field = line.substr(0, end);
	unsigned int i = 0;
	while (i < field.length())
	{
		field[i] = tolower(field[i]);
		i++;
	}
	return (field);
}

std::string parseHeaderValue(std::string &line) {
	size_t start = line.find_first_of(" ", 0);
	if (start == std::string::npos || line.length() == start + 1)
		return (std::string());
	return (line.substr(start + 1));
}

int Request::checkMethod(std::string &meth) {
	if (meth == "GET" || meth == "POST" || meth == "DELETE")
		return (0);
	if (meth == "HEAD" || meth == "PUT" || meth == "CONNECT" || meth == "OPTIONS" || meth == "TRACE" || meth == "PATCH")
		return (-501);
	return (-400);
}

int Request::checkURI(std::string &uri) {
	if (uri.length() == 0 || uri[0] != '/')
		return (-400);
	std::vector<std::string> list = split(uri, '/');
	int count = 0;
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
		if (*it == "..")
			count--;
		if (count < 0)
			return (-400);
		if (*it != "." && it->length() != 0)
			count++;
	}
	return (0);
}

int Request::checkVersion(std::string &version) {
	if (version == "HTTP/1.1")
		return (0);
	if (version == "HTTP/1.0" || version == "HTTP/0.9")
		return (-505);
	return (-400);
}

int Request::checkFirstLine(std::string line) {
	std::vector<std::string> list = split(line, ' ');
	if (list.size() != 3)
		return (-400);
	int ret = checkMethod(list[0]);
	if (ret < 0)
		return (ret);
	_method = list[0];
	ret = checkURI(list[1]);
	if (ret < 0)
		return (ret);
	_uri = list[1];
	ret = checkVersion(list[2]);
	if (ret < 0)
		return (ret);	
	_version = list[2];
	return (0);
}

int Request::parseFirstLine(Buffer &buf) {
	int status;
	std::string line = buf.getLine(status);
	if (status < 0)
		return (0);
	if (line.length() == 0) {
		buf.erase(buf.getPos());
		return (0);
	}
	status = checkFirstLine(line);
	if (status < 0)
		return (status);
	return (buf.getPos());
}

int Request::parseHeaderLine(Buffer &buf) {
	int status;
	std::string line = buf.getLine(status);
	if (status != 0)
		return (-1);
	if (line.length() == 0)
		return (1);
	std::string field = parseHeaderField(line);
	if (field[field.length() - 1] != ':') {
		line = buf.getLine(status);
		return (0);
	}
	field.erase(field.length() - 1);
	std::string value = parseHeaderValue(line);
	if (value.length() == 0) {
		line = buf.getLine(status);
		return (0);
	}
	if (_headers.find(field) != _headers.end())
		_headers.erase(field);
	_headers.insert(std::pair<std::string, std::string>(field, value));
	return (0);
}

int Request::parseContent(Buffer &buf, unsigned int maxBodySize) {
	if (_headers.find("content-length") != _headers.end()) {
		std::string value = _headers.find("content-length")->second;
		if (value.find_first_not_of("0123456789") == std::string::npos)
			return (readContent(buf, maxBodySize, atoi(value.c_str())));
		else
			return (-400);
	}
	if (_headers.find("transfer-encoding") != _headers.end()) {
		std::string value = _headers.find("transfer-encoding")->second;
		if (value == "chunked")
			return (readContentChunked(buf, maxBodySize));
		else
			return (-400);
	}
	return (buf.getPos());
}

int Request::readContent(Buffer &buf, unsigned int maxBodySize, int contentLength) {
	if (static_cast<unsigned int>(contentLength) > maxBodySize)
		return (-413);
	if ((buf.getSize() - buf.getPos()) < static_cast<unsigned int>(contentLength))
		return (0);
	for (int i = 0; contentLength > i; ++i) {
		_content.push_back(buf.getContent()[buf.getPos() + i]);
	}
	buf.setPos(buf.getPos() + contentLength);
	return (buf.getPos());
}

int atouBase(std::string &str, std::string base) {
	int res = 0;
	unsigned int end = str.find_first_not_of(base);
	if (end == 0)
		return (-1);
	for (unsigned int i = 0; i < end && i < str.length(); ++i) {
		res *= base.length();
		res += base.find(str[i]);
	}
	return (res);
}

int Request::readContentChunked(Buffer &buf, unsigned int maxBodySize) {
	int status;
	unsigned int oldPos = buf.getPos();
	std::string line = buf.getLine(status);
	while (status == 0 && line.length() != 0) {
		int toRead = atouBase(line, "0123456789ABCDEF");
		if (toRead < 0)
			return (-400);
		if ((buf.getSize() - buf.getPos()) < (static_cast<unsigned int>(toRead) + 2))
			return (0);
		if (toRead == 0) {
			if (buf.getContent()[buf.getPos()] != '\r' || buf.getContent()[buf.getPos() + 1] != '\n')
				return (-400);
			buf.setPos(buf.getPos() + 2);
			break;
		}
		if ((buf.getPos() - oldPos) > maxBodySize)
			return (-413);
		for (int i = 0; i < toRead; ++i) {
			_content.push_back(buf.getContent()[buf.getPos() + i]);
		}
		buf.setPos(buf.getPos() + toRead + 2);
		line = buf.getLine(status);
	}
	if (status < 0)
		return (0);
	return (buf.getPos());
}
