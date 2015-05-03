#pragma once

typedef unsigned long BytesCnt;
typedef unsigned long EntryNum;
const unsigned int FNAMELEN=8;
const unsigned int FEXTLEN=3;
struct Entry {
	static const char ATT_FILE = 1;
	static const char ATT_DIRECTORY = 2;
	char name[ FNAMELEN];
	char ext[ FEXTLEN];
	char attributes;
	unsigned long firstCluster;
	unsigned long size;
} ;