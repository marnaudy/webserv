#include "Buffer.hpp"

Buffer::Buffer() : _content(NULL), _size(0), _pos(0) {}

Buffer::~Buffer() {
	delete[] _content;
}

char *Buffer::getContent() {
	return (_content);
}

size_t Buffer::getSize() {
	return (_size);
}

size_t Buffer::getPos() {
	return (_pos);
}

void Buffer::setPos(size_t pos) {
	_pos = pos;
}

std::string Buffer::getLine(int &status) {
	size_t oldPos = _pos;
	while (_pos + 1 < _size) {
		if (_content[_pos] == '\r' && _content[_pos + 1] == '\n') {
			status = 0;
			_pos += 2;
			return (std::string(&_content[oldPos], _pos - 2 - oldPos));
		}
		_pos++;
	}
	status = -1;
	return (std::string());
}

void Buffer::addToBuffer(char *toAdd, size_t sizeAdd) {
	char *newContent = new char[_size + sizeAdd]();
	for (unsigned int i = 0; i < _size; ++i) {
		newContent[i] = _content[i];
	}
	for (unsigned int i = 0; i < sizeAdd; ++i) {
		newContent[_size + i] = toAdd[i];
	}
	delete[] _content;
	_size += sizeAdd;
	_content = newContent;
}

void Buffer::erase(size_t len) {
	size_t newSize = (_size >= len) ? _size - len : 0;
	char *newContent = new char[newSize]();
	for (unsigned int i = 0; i < newSize; ++i) {
		newContent[i] = _content[len + i];
	}
	delete[] _content;
	_size = newSize;
	_content = newContent;
}
