#pragma once

#include "common.h"
#include "part.h"

class FAT;
class File;

class KernelFile {
public:
	static const char MODE_READ = 'r';
	static const char MODE_WRITE = 'w';
	static const char MODE_APPEND = 'a';

	void write (BytesCnt, char* buffer) ;
	BytesCnt read (BytesCnt, char* buffer);
	void seek (BytesCnt);
	BytesCnt filePos();
	char eof ();
	BytesCnt getFileSize ();
	~KernelFile() ;
	KernelFile (Partition* part, FAT* fat, Entry entry, char mode, char* fileName);
private:
	Partition *part;
	FAT *fat;
	Entry entry;
	char mode;
	BytesCnt cursor;
	char loadedCluster[ClusterSize];
	ClusterNo cursorClusterIndex;
	EntryNum cursorClusterFatEntry;
	char *fileName;
};