#include "fat.h"

#define nextEntry(x) entries[x]

FAT::FAT(Partition* part, bool doFormat = false) {
	partition = part;
	ClusterNo totalClusters = partition->getNumOfClusters();
	clusterZero = new ClusterZero(totalClusters);
	entries = new EntryNum[((clusterZero->fatEntryCount - 1) / FatEntriesPerCluster + 1) * FatEntriesPerCluster]; // ceiling(entries / entriesPerFatC) * entriesPerFatC
	if (doFormat) {
		format();
	} else {
		load();
	}
}

void FAT::format() {
	nextEntry(0) = 0;
	for (EntryNum entry = 1; entry < clusterZero->fatEntryCount - 1; entry++) {
		nextEntry(entry) = entry + 1;
	}
	nextEntry(clusterZero->fatEntryCount - 1) = 0;
	save();
}

EntryNum FAT::allocate(EntryNum clusterCount) {
	EntryNum firstFree = clusterZero->firstFree;
	EntryNum currentEntry = firstFree;
	if (clusterCount == 0) {
		throw "clusterCount must be positive";
	}
	for (; clusterCount > 1; clusterCount--) {
		if (nextEntry(currentEntry) == 0) {
			throw "Not enough space in file allocation table";
		}
		if (nextEntry(currentEntry) >= clusterZero->fatEntryCount) {
			throw "Entry number out of range";
		}
		currentEntry = nextEntry(currentEntry);
	}
	clusterZero->firstFree = nextEntry(currentEntry);
	save();
	return firstFree;
}

void FAT::reallocate(EntryNum firstEntry, EntryNum clusterCount) {
	EntryNum currentEntry = firstEntry;
	EntryNum currentClusterCount = 1;
	if (clusterCount == 0) {
		throw "clusterCount must be positive";
	}
	while ((nextEntry(currentEntry) != 0) && (currentClusterCount != clusterCount)) {
		currentEntry = nextEntry(currentEntry);
		currentClusterCount++;
	}
	if ((nextEntry(currentEntry) == 0) && (currentClusterCount == clusterCount)) {
		return;
	}
	if (nextEntry(currentEntry) == 0) {
		nextEntry(currentEntry) = allocate(clusterCount - currentClusterCount);
	} else if (currentClusterCount == clusterCount) {
		EntryNum entryToFree = nextEntry(currentEntry);
		nextEntry(currentEntry) = 0;
		free(entryToFree);
	}
}

void FAT::free(EntryNum entry) {
	EntryNum newFirstFree = entry;
	while (nextEntry(entry) != 0) {
		entry = nextEntry(entry);
	}
	nextEntry(entry) = clusterZero->firstFree;
	clusterZero->firstFree = newFirstFree;
	save();
}

ClusterNo FAT::getCluster(EntryNum entry) {
	if (entry >= clusterZero->fatEntryCount) {
		throw "Entry number out of range";
	}
	return 1 + getFatSize() + entry;
}

EntryNum FAT::getNextEntry(EntryNum entry) {
	if (nextEntry(entry) == 0) {
		throw "No more entries";
	}
	return nextEntry(entry);
}

EntryNum FAT::getNumOfEntries(EntryNum entry) {
	EntryNum count = 1;
	while (nextEntry(entry) != 0) {
		entry = nextEntry(entry);
		count++;
	}
	return count;
}

Entry FAT::getClusterZeroEntry() {
	Entry result = {
		"       ",
		"  ",
		2,
		clusterZero->firstRoot,
		clusterZero->rootEntryCount
	};
	return result;
}

FAT::~FAT() {
	delete []entries;
	delete clusterZero;
}

ClusterNo FAT::getFatSize() {
	long fatSize = clusterZero->fatEntryCount - 1;
	fatSize /= (long)FatEntriesPerCluster;
	return fatSize + 1;
}

void FAT::save() {
	partition->writeCluster(0, (char*)clusterZero);
	for (ClusterNo fatCluster = 0; fatCluster < getFatSize(); fatCluster++) {
		int result = partition->writeCluster(fatCluster + 1, (char*)entries + (fatCluster * FatEntriesPerCluster));
		if (!result) {
			throw "Write to cluster failed";
		}
	}
}

void FAT::load() {
	partition->readCluster(0, (char*)clusterZero);
	for (ClusterNo fatCluster = 0; fatCluster < getFatSize(); fatCluster++) {
		int result = partition->readCluster(fatCluster + 1, (char*)entries + (fatCluster * FatEntriesPerCluster));
		if (!result) {
			throw "Read from cluster failed";
		}
	}
}