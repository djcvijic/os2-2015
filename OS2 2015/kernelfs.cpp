#include "kernelfs.h"
#include "fat.h"
#include "directory.h"
#include "path.h"
#include "file.h"
#include "kernelfile.h"
#include <iostream>

KernelFS* KernelFS::getInstance() {
	if (KernelFS::instance == 0) {
		KernelFS::instance = new KernelFS();
	}
	return KernelFS::instance;
}

KernelFS::~KernelFS () {
	for (int i = 0; i < MaxPartitions; i++) {
		if (partitions[i] != 0) {
			delete partitions[i];
		}
		if (fats[i] != 0) {
			delete fats[i];
		}
		if (rootDirs[i] != 0) {
			delete rootDirs[i];
		}
	}
	openFiles.clear();
}

char KernelFS::mount(Partition* partition) {
	for (int i = 0; i < MaxPartitions; i++) {
		if (partitions[i] == 0) {
			partitions[i] = partition;
			try {
				fats[i] = new FAT(partitions[i], false);
				rootDirs[i] = new Directory(partitions[i], fats[i], 0, false);
			} catch (const char* e) {
				std::cout << "FAT could not be loaded for partition " << partITOA(i) << ": " << e << '\n';
			}
			return partITOA(i);
		}
	}
	throw "Max partitions reached";
}

void KernelFS::unmount(char part) {
	int partIndex = partATOI(part);
	if (partitions[partIndex] == 0) {
		throw "Partition is not mounted";
	}
	partitions[partIndex] = 0;
	delete fats[partIndex];
	delete rootDirs[partIndex];
	fats[partIndex] = 0;
	rootDirs[partIndex] = 0;
}

void KernelFS::format(char part) {
	int partIndex = partATOI(part);
	if (partitions[partIndex] == 0) {
		throw "Partition is not mounted";
	}
	if (fats[partIndex] == 0) {
		fats[partIndex] = new FAT(partitions[partIndex], true);
	} else {
		fats[partIndex]->format();
		delete rootDirs[partIndex];
	}
	rootDirs[partIndex] = new Directory(partitions[partIndex], fats[partIndex], 0, true);
}

char KernelFS::doesExist(char* fname) {
	Path* fullPath = 0;
	Directory* containingDir = 0;
	try {
		fullPath = new Path(fname);
		containingDir = findContainingDirectory(fname);
		containingDir->getEntry(fullPath->getBaseName());
		return 1;
	} catch (const char* e) {
		std::cout << "File not found: " << fname << '\n' << e << '\n';
		return 0;
	} finally {
		if (fullPath != 0) {
			delete fullPath;
		}
		if (containingDir != 0) {
			delete containingDir;
		}
	}
}

File* KernelFS::open(char* fname, char mode) {
	if (openFiles.find(fname) != openFiles.end()) {
		throw "File is still open";
	}
	File *result = new File();
	Path *fullPath = new Path(fname);
	int partIndex = partATOI(fullPath->getPartition());
	Directory *containingDir = findContainingDirectory(fname);
	Entry fileEntry;
	if (mode == KernelFile::MODE_WRITE) {
		try {
			fileEntry = containingDir->addEntry(fullPath->getBaseName());
		} catch (const char* e) {
			std::cout << "File already exists, deleting... " << e << '\n';
			containingDir->removeEntry(fullPath->getBaseName());
			fileEntry = containingDir->addEntry(fullPath->getBaseName());
		}
	} else {
		fileEntry = containingDir->getEntry(fullPath->getBaseName());
	}
	delete fullPath;
	delete containingDir;
	result->myImpl = new KernelFile(partitions[partIndex], fats[partIndex], fileEntry, mode, fname);
	openFiles[fname] = result;
	return result;
}

void KernelFS::deleteFile(char* fname) {
	if (openFiles.find(fname) != openFiles.end()) {
		throw "File is still open";
	}
	Path *fullPath = new Path(fname);
	int partIndex = partATOI(fullPath->getPartition());
	Directory *containingDir = findContainingDirectory(fname);
	containingDir->removeEntry(fullPath->getBaseName());
	delete fullPath;
	delete containingDir;
}

void KernelFS::readDir(char* dirname, EntryNum n, Entry &e) {
	char result = 1;
	Directory *containingDir = findContainingDirectory(dirname, true);
	e = containingDir->getEntry(n);
	delete containingDir;
}

void KernelFS::fileClosed(char* fname) {
	openFiles.erase(fname);
}

KernelFS::KernelFS() {
	for (int i = 0; i < MaxPartitions; i++) {
		partitions[i] = 0;
		fats[i] = 0;
		rootDirs[i] = 0;
	}
}

char KernelFS::partITOA(int partIndex) {
	if (partIndex < 0 || partIndex > MaxPartitions) {
		throw "Invalid partition";
	}
	return 'A' + partIndex;
}

int KernelFS::partATOI(char partLetter) {
	int partIndex = partLetter - 'A';
	if (partIndex < 0 || partIndex > MaxPartitions) {
		throw "Invalid partition";
	}
	return partIndex;
}

Directory* KernelFS::findContainingDirectory(char* fname, bool isContainingDirectory) {
	Path *currentPath = new Path(fname),
		*oldPath;
	if (!currentPath->isAbsolute()) {
		throw "Provided path is not absolute";
	}
	int partIndex = partATOI(currentPath->getPartition());
	Entry currentDirEntry = fats[partIndex]->getClusterZeroEntry();
	EntryNum currentEntry = currentDirEntry.firstCluster;
	while (true) {
		oldPath = currentPath;
		currentPath = oldPath->getNextLevelPath();
		delete oldPath;
		Directory *currentDir = new Directory(partitions[partIndex], fats[partIndex], currentEntry, false);
		if (currentPath->isBaseName() && isContainingDirectory) {
			delete currentDir;
			return new Directory(partitions[partIndex], fats[partIndex], currentDirEntry.firstCluster, false);
		}
		currentDirEntry = currentDir->getEntry(currentPath->getTopLevelDirectory());
		delete currentDir;
		if (currentPath->isBaseName() && !isContainingDirectory) {
			return new Directory(partitions[partIndex], fats[partIndex], currentDirEntry.firstCluster, false);
		}
		currentEntry = currentDirEntry.firstCluster;
	}
}

KernelFS *KernelFS::instance = 0;