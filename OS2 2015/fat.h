#pragma once

#include "part.h"
#include "common.h"

const unsigned long FatEntriesPerCluster = ClusterSize / 4;
struct ClusterZero {
	EntryNum firstFree;
	EntryNum fatEntryCount;
	EntryNum firstRoot;
	EntryNum rootEntryCount;
	char padding[ClusterSize - 4 * sizeof(EntryNum)];

	ClusterZero(EntryNum totalClusters) {
		fatEntryCount = (totalClusters - 1 - 1) / (FatEntriesPerCluster + 1) + 1; // ceiling((totalCs - zerothC) / (dataCsPerFatC + FatC))
		firstRoot = 1 + (fatEntryCount - 1) / FatEntriesPerCluster + 1; // zerothC + ceiling(dataCs / dataCsPerFatC)
		rootEntryCount = 0;
		firstFree = firstRoot + 1;
	}
};

class FAT {
public:
	FAT(Partition* part, bool doFormat);
	void format();
	EntryNum allocate(EntryNum clusterCount);
	void reallocate(EntryNum firstEntry, EntryNum clusterCount);
	void free(EntryNum entry);
	ClusterNo getCluster(EntryNum entry);
	EntryNum getNextEntry(EntryNum entry);
	EntryNum getNumOfEntries(EntryNum entry);
	Entry getClusterZeroEntry();
	~FAT();
private:
	EntryNum *entries;
	Partition *partition;
	ClusterZero *clusterZero;

	ClusterNo getFatSize();
	void save();
	void load();
};