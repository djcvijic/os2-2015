#include "directory.h"
#include "fat.h"
#include "path.h"
#include <cstring>
#include <stdexcept>

#define entriesTail entries[entryCount]

Directory::Directory(Partition* part, FAT* fat, EntryNum fatEntry, bool doInit) {
	partition = part;
	this->fat = fat;
	this->fatEntry = fatEntry;
	entries = 0;
	if (doInit) {
		entryCount = 0;
		entries = new Entry[DirectoryEntriesPerCluster];
		size = 1;
		for (EntryNum i = 0; i < DirectoryEntriesPerCluster; i++) {
			entries[i].name[0] = 0;
		}
	} else {
		load();
	}
}

Entry Directory::addEntry(char* fname) {
	if (find(fname) >= 0) {
		throw "Entry already exists in this directory";
	}
	Path *path = new Path(fname);
	addPathToEntry(&entriesTail, path);
	delete path;
	entriesTail.attributes = Entry::ATT_FILE;
	entriesTail.firstCluster = fat->allocate(1);
	entriesTail.size = 0;
	Entry result = entriesTail;
	entryCount++;
	reallocate();
	save();
	return result;
}

void Directory::updateEntry(char* fname, ClusterNo firstCluster, BytesCnt size) {
	long found = find(fname);
	if (found == -1) {
		throw "Entry not found";
	}
	entries[found].firstCluster = firstCluster;
	entries[found].size = size;
	save();
}

void Directory::removeEntry(char* fname) {
	long found = find(fname);
	if (found == -1) {
		throw "Entry not found";
	}
	entries[found].name[0] = 0;
	entries[found] = entriesTail;
	fat->free(entriesTail.firstCluster);
	entryCount--;
	reallocate();
	save();
}

Entry Directory::getEntry(char* fname) {
	long found = find(fname);
	if (found == -1) {
		throw "Entry not found";
	}
	return entries[found];
}

Entry Directory::getEntry(EntryNum n) {
	if (n < 0 || n >= entryCount) {
		throw std::out_of_range("Entry not found");
	}
	return entries[n];
}

Directory::~Directory() {
	delete []entries;
}

long Directory::find(char* fname) {
	Path *path = new Path(fname);
	long found = -1;
	for (EntryNum i = 0; i < entryCount; i++) {
		if (comparePathWithEntry(entries + i, path)) {
			found = i;
			break;
		}
	}
	delete path;
	return found;
}

void Directory::reallocate() {
	ClusterNo requiredSize = 1 + entryCount / DirectoryEntriesPerCluster;
	if (requiredSize != size) {
		fat->reallocate(fatEntry, requiredSize);
		EntryNum newMaxEntryCount = requiredSize * DirectoryEntriesPerCluster;
		Entry *tempEntriesArray = new Entry[newMaxEntryCount];
		for (EntryNum i = 0; i < newMaxEntryCount; i++) {
			if (i < entryCount) {
				tempEntriesArray[i] = entries[i];
			} else {
				tempEntriesArray[i].name[0] = '\0';
			}
		}
		delete []entries;
		entries = tempEntriesArray;
		save();
	}
}

void Directory::save() {
	ClusterNo currentEntry = fatEntry;
	for (ClusterNo i = 0; i < size; i++) {
		if (currentEntry == 0) {
			throw "Tried getting invalid entry";
		}
		int result = partition->writeCluster(fat->getCluster(currentEntry), (char*) (entries + i * DirectoryEntriesPerCluster));
		if (!result) {
			throw "Write to cluster failed";
		}
		currentEntry = fat->getNextEntry(currentEntry);
	}
}

void Directory::load() {
	if (entries != 0) {
		delete []entries;
	}
	size = fat->getNumOfEntries(fatEntry);
	entries = new Entry[size * DirectoryEntriesPerCluster];
	EntryNum currentEntry = fatEntry;
	for (ClusterNo i = 0; i < size; i++) {
		if (currentEntry == 0) {
			throw "Tried getting invalid entry";
		}
		int result = partition->readCluster(fat->getCluster(currentEntry), (char*)entries + (i * DirectoryEntriesPerCluster));
		if (!result) {
			throw "Read from cluster failed";
		}
		currentEntry = fat->getNextEntry(currentEntry);
	}
}

void Directory::addPathToEntry(Entry* entry, Path* path) {
	char *fname = path->getBaseNameWOExtension(),
		*fext = path->getExtension();
	for (unsigned long i = 0; i < FNAMELEN; i++) {
		if (i < strlen(fname)) {
			entry->name[i] = fname[i];
		} else {
			entry->name[i] = ' ';
		}
	}
	for (unsigned long j = 0; j < FEXTLEN; j++) {
		if (j < strlen(fext)) {
			entry->ext[j] = fext[j];
		} else {
			entry->ext[j] = ' ';
		}
	}
}

bool Directory::comparePathWithEntry(Entry* entry, Path* path) {
	char *fname = path->getBaseNameWOExtension(),
		*fext = path->getExtension();
	int fnamelen = strlen(fname),
		fextlen = strlen(fext);
	for (int i = 0; i < fnamelen; i++) {
		if ((i >= FNAMELEN) || (entry->name[i] == ' ') || (fname[i] != entry->name[i])) {
			return false;
		}
	}
	for (int j = 0; j < fextlen; j++) {
		if ((j >= FEXTLEN) || (entry->ext[j] == ' ') || (fext[j] != entry->ext[j])) {
			return false;
		}
	}
	return true;
}