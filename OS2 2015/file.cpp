#include "file.h"
#include "kernelfile.h"
#include <iostream>

char File::write (BytesCnt n, char* buffer) {
	try {
		myImpl->write(n, buffer);
		return 1;
	} catch (const char* e) {
		std::cerr << "Error writing file: " << e << '\n';
		return 0;
	}
}
BytesCnt File::read (BytesCnt n, char* buffer) {
	try {
		return myImpl->read(n, buffer);
	} catch (const char* e) {
		std::cerr << "Error reading file: " << e << '\n';
		return 0;
	}
}
char File::seek (BytesCnt n) {
	try {
		myImpl->seek(n);
		return 1;
	} catch (const char* e) {
		std::cerr << "Error seeking file: " << e << '\n';
		return 0;
	}
}
BytesCnt File::filePos() {
	return myImpl->filePos();
}
char File::eof () {
	return myImpl->eof();
}
BytesCnt File::getFileSize () {
	return myImpl->getFileSize();
}
File::~File() {
	delete myImpl;
}
File::File () {
}