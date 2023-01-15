#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <string>

#define READ_SIZE 10000

class Buffer {
public:
	Buffer();
	~Buffer();
	char *getContent();
	size_t getSize();
	void addToBuffer(char *toAdd, size_t sizeAdd);
	void erase(size_t len);
private:
	char *_content;
	size_t _size;
};

#endif
