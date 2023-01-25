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

unsigned int Response::getCode() {
	return (_code);
}

void Response::printContent() {
	write(1, &_content[0], _content.size());
	std::cout << std::endl;
}

void Response::addHeader(std::string field, std::string value) {
	if (_headers.find(field) != _headers.end())
		_headers.erase(field);
	_headers.insert(std::pair<std::string, std::string>(field, value));
}

void Response::addHeaderLine(std::string line) {
	_headerLines.push_back(line);
}

void Response::readFileContent(std::ifstream &ifs) {
	char c;
	std::ostringstream ss;
	_content = std::vector<char>();
	while (!ifs.eof()) {
		c = ifs.get();
		if (!ifs.eof())
			_content.push_back(c);
	}
	ss << _content.size();
	_headers["content-length"] = ss.str();
}

void Response::setContent(std::string &str) {
	_content.insert(_content.begin(), str.begin(), str.end());
	std::ostringstream ss;
	ss << _content.size();
	_headers["content-length"] = ss.str();
}

void Response::setContent(char *buf, size_t size) {
	for (unsigned int i = 0; i < size; i++)
		_content.push_back(buf[i]);
	std::ostringstream ss;
	ss << _content.size();
	_headers["content-length"] = ss.str();
}

void Response::setContentType(std::string fileName) {
	size_t start = fileName.find_last_of(".");
	_headers["content-type"] = "text";
	std::string ext = fileName.substr(start + 1);
	if (ext == "aac")
		_headers["content-type"] = "audio/aac";
	if (ext == "abw")
		_headers["content-type"] = "application/x-abiword";
	if (ext == "arc")
		_headers["content-type"] = "application/x-freearc";
	if (ext == "avif")
		_headers["content-type"] = "image/avif";
	if (ext == "avi")
		_headers["content-type"] = "video/x-msvideo";
	if (ext == "azw")
		_headers["content-type"] = "application/vnd.amazon.ebook";
	if (ext == "bin")
		_headers["content-type"] = "application/octet-stream";
	if (ext == "bmp")
		_headers["content-type"] = "image/bmp";
	if (ext == "bz")
		_headers["content-type"] = "application/x-bzip";
	if (ext == "bz2")
		_headers["content-type"] = "application/x-bzip2";
	if (ext == "cda")
		_headers["content-type"] = "application/x-cdf";
	if (ext == "csh")
		_headers["content-type"] = "application/x-csh";
	if (ext == "css")
		_headers["content-type"] = "text/css";
	if (ext == "csv")
		_headers["content-type"] = "text/csv";
	if (ext == "doc")
		_headers["content-type"] = "application/msword";
	if (ext == "docx")
		_headers["content-type"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	if (ext == "eot")
		_headers["content-type"] = "application/vnd.ms-fontobject";
	if (ext == "epub")
		_headers["content-type"] = "application/epub+zip";
	if (ext == "gz")
		_headers["content-type"] = "application/gzip";
	if (ext == "epub")
		_headers["content-type"] = "application/epub+zip";
	if (ext == "gif")
		_headers["content-type"] = "image/gif";
	if (ext == "htm" || ext == "html")
		_headers["content-type"] = "text/html";
	if (ext == "ico")
		_headers["content-type"] = "image/vnd.microsoft.icon";
	if (ext == "ics")
		_headers["content-type"] = "text/calendar";
	if (ext == "jar")
		_headers["content-type"] = "application/java-archive";
	if (ext == "jpg" || ext == "jpeg")
		_headers["content-type"] = "image/jpeg";
	if (ext == "js")
		_headers["content-type"] = "text/javascript";
	if (ext == "json")
		_headers["content-type"] = "application/json";
	if (ext == "jsonls")
		_headers["content-type"] = "application/ld+json";
	if (ext == "mid" || ext == "midi")
		_headers["content-type"] = "audio/midi";
	if (ext == "mjs")
		_headers["content-type"] = "text/javascript";
	if (ext == "mp3")
		_headers["content-type"] = "audio/mpeg";
	if (ext == "mp4")
		_headers["content-type"] = "video/mp4";
	if (ext == "mpeg")
		_headers["content-type"] = "video/mpeg";
	if (ext == "mpkg")
		_headers["content-type"] = "application/vnd.apple.installer+xml";
	if (ext == "ods")
		_headers["content-type"] = "application/vnd.oasis.opendocument.spreadsheet";
	if (ext == "odt")
		_headers["content-type"] = "application/vnd.oasis.opendocument.text";
	if (ext == "odp")
		_headers["content-type"] = "application/vnd.oasis.opendocument.presentation";
	if (ext == "oga")
		_headers["content-type"] = "audio/ogg";
	if (ext == "ogv")
		_headers["content-type"] = "video/ogg";
	if (ext == "ogx")
		_headers["content-type"] = "application/ogg";
	if (ext == "opus")
		_headers["content-type"] = "audio/opus";
	if (ext == "otf")
		_headers["content-type"] = "font/otf";
	if (ext == "png")
		_headers["content-type"] = "image/png";
	if (ext == "pdf")
		_headers["content-type"] = "application/pdf";
	if (ext == "php")
		_headers["content-type"] = "application/x-httpd-php";
	if (ext == "ppt")
		_headers["content-type"] = "application/vnd.ms-powerpoint";
	if (ext == "pptx")
		_headers["content-type"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	if (ext == "rar")
		_headers["content-type"] = "application/vnd.rar";
	if (ext == "rtf")
		_headers["content-type"] = "application/rtf";
	if (ext == "sh")
		_headers["content-type"] = "application/x-sh";
	if (ext == "svg")
		_headers["content-type"] = "image/svg+xml";
	if (ext == "tar")
		_headers["content-type"] = "application/x-tar";
	if (ext == "tif" || ext == "tiff")
		_headers["content-type"] = "image/tiff";
	if (ext == "ts")
		_headers["content-type"] = "video/mp2t";
	if (ext == "ttf")
		_headers["content-type"] = "font/ttf";
	if (ext == "txt")
		_headers["content-type"] = "text/plain";
	if (ext == "vsd")
		_headers["content-type"] = "application/vnd.visio";
	if (ext == "wav")
		_headers["content-type"] = "audio/wav";
	if (ext == "weba")
		_headers["content-type"] = "audio/webm";
	if (ext == "webm")
		_headers["content-type"] = "video/webm";
	if (ext == "webp")
		_headers["content-type"] = "image/webp";
	if (ext == "woff")
		_headers["content-type"] = "font/woff";
	if (ext == "woff2")
		_headers["content-type"] = "font/woff2";
	if (ext == "xhtml")
		_headers["content-type"] = "application/xhtml+xml";
	if (ext == "xls")
		_headers["content-type"] = "application/vnd.ms-excel";
	if (ext == "xlsx")
		_headers["content-type"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	if (ext == "xml")
		_headers["content-type"] = "application/xml";
	if (ext == "xul")
		_headers["content-type"] = "application/vnd.mozilla.xul+xml";
	if (ext == "zip")
		_headers["content-type"] = "application/zip";
	if (ext == "3gp")
		_headers["content-type"] = "video/3gpp";
	if (ext == "3g2")
		_headers["content-type"] = "video/3gpp2";
	if (ext == "7z")
		_headers["content-type"] = "application/x-7z-compressed";
}

size_t Response::exprt(char **buffer) {
	std::string message;
	std::ostringstream ss;
	ss << _code;
	message = "HTTP/1.1 " + ss.str() + " " + _message + "\r\n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
		message += it->first + ": " + it->second + "\r\n";
	}
	for (unsigned int i = 0; i < _headerLines.size(); ++i) {
		message += _headerLines[i] + "\r\n";
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
