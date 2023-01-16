#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <string>
#include <iostream>

#define READ_SIZE 10000

class Buffer {
public:
	Buffer();
	~Buffer();
	char *getContent();
	size_t getSize();
	size_t getPos();
	void setPos(size_t pos);
	std::string getLine(int &status);
	void addToBuffer(char *toAdd, size_t sizeAdd);
	void erase(size_t len);
private:
	char *_content;
	size_t _size;
	size_t _pos;
};

#endif
