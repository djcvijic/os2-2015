#pragma once

#include "part.h"
#include "common.h"

class FAT;
class Path;

class Directory {
public:
	Directory(Partition* part, FAT* fat, EntryNum fatEntry, bool doInit);
	Entry addEntry(char* fname);
	void updateEntry(char* fname, ClusterNo firstCluster, BytesCnt size);
	void removeEntry(char* fname);
	Entry getEntry(char* fname);
	Entry getEntry(EntryNum n);
	~Directory();
private:
	static const unsigned long DirectoryEntriesPerCluster = ClusterSize / sizeof(Entry); 

	Entry *entries;
	EntryNum entryCount;
	ClusterNo size;
	Partition *partition;
	FAT *fat;
	EntryNum fatEntry;

	long find(char* fname);
	void reallocate();
	void save();
	void load();
	void addPathToEntry(Entry* entry, Path* path);
	bool comparePathWithEntry(Entry* entry, Path* path);
};