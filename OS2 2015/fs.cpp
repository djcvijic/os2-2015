#include "fs.h"
#include "kernelfs.h"
#include <iostream>

FS::~FS () {
}
char FS::mount(Partition* partition) {
	return myImpl->mount(partition);
}
char FS::unmount(char part) {
	try {
		myImpl->unmount(part);
		return 1;
	} catch (const char* e) {
		std::cerr << "Error unmounting partition: " << e << '\n';
		return 0;
	}
}
char FS::format(char part) {
	try {
		myImpl->format(part);
		return 1;
	} catch (const char* e) {
		std::cerr << "Error formatting partition: " << e << '\n';
		return 0;
	}
}
char FS::doesExist(char* fname) {
	return myImpl->doesExist(fname);
}
File* FS::open(char* fname, char mode) {
	return myImpl->open(fname, mode);
}
char FS::deleteFile(char* fname) {
	try {
		myImpl->deleteFile(fname);
		return 1;
	} catch (const char* e) {
		std::cerr << "Error deleting file: " << e << '\n';
		return 0;
	}
}
char FS::readDir(char* dirname, EntryNum n, Entry &e) {
	try {
		myImpl->readDir(dirname, n, e);
		return 1;
	} catch (const char* e) {
		std::cerr << "Error reading directory: " << e << '\n';
		return 0;
	} catch (const std::out_of_range& e) {
		std::cerr << "Error reading directory: " << e.what() << '\n';
		return 2;
	}
}
FS::FS () {
}
KernelFS *FS::myImpl = KernelFS::getInstance();