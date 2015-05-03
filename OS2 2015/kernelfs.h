#pragma once

#include "common.h"
#include <map>

struct FileNameComparer {
	bool operator()(const char* x, const char* y) { return strcmp(x, y) == 0; }
};

class Partition;
class File;
class FAT;
class Directory;

class KernelFS {
public:
	static KernelFS* getInstance();

	~KernelFS ();
	char mount(Partition* partition);
	void unmount(char part);
	void format(char part);
	char doesExist(char* fname);
	File* open(char* fname, char mode);
	void deleteFile(char* fname);
	void readDir(char* dirname, EntryNum n, Entry &e);
	void fileClosed(char* fname);
private:
	static const unsigned int MaxPartitions = 26;

	static KernelFS* instance;
	Partition* partitions[MaxPartitions];
	FAT* fats[MaxPartitions];
	Directory* rootDirs[MaxPartitions];
	std::map<char*, File*, FileNameComparer> openFiles;

	KernelFS();
	char partITOA(int partIndex);
	int partATOI(char partLetter);
	Directory* findContainingDirectory(char* fname, bool isContainingDirectory = false);
};