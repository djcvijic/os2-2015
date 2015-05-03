#include "kernelfile.h"
#include "fat.h"
#include "file.h"
#include "kernelfs.h"
#include <algorithm>

void KernelFile::write (BytesCnt bytesCnt, char* buffer) {
	BytesCnt newPos = cursor + bytesCnt;
	if (newPos > entry.size) {
		fat->reallocate(entry.firstCluster, newPos);
	}
	ClusterNo clustersToWrite = 1 + newPos / ClusterSize - cursor / ClusterSize;
	for (ClusterNo clusterIndex = 0; clusterIndex < clustersToWrite; clusterIndex++) {
		BytesCnt startOffset = 0, endOffset = ClusterSize;
		bool firstOrLast = false;
		if (clusterIndex == 0) {
			startOffset = cursor % ClusterSize;
			firstOrLast = true;
		}
		if (clusterIndex == clustersToWrite - 1) {
			endOffset = newPos % ClusterSize;
			firstOrLast = true;
		}
		if (firstOrLast) {
			int readResult = part->readCluster(fat->getCluster(cursorClusterFatEntry), loadedCluster);
			if (!readResult) {
				throw "Read from cluster failed";
			}
		}
		std::copy(buffer, buffer + endOffset, loadedCluster + startOffset);
		buffer += ClusterSize;
		int writeResult = part->writeCluster(fat->getCluster(cursorClusterFatEntry), loadedCluster);
		if (!writeResult) {
			throw "Write to cluster failed";
		}
		cursorClusterFatEntry = fat->getNextEntry(cursorClusterFatEntry);
	}
	entry.size = cursor = newPos;
	cursorClusterIndex = cursor / ClusterSize;
}

BytesCnt KernelFile::read (BytesCnt bytesCnt, char* buffer) {
	if (eof()) {
		throw "Cannot read from end of file";
	}
	BytesCnt newPos = cursor + bytesCnt;
	if (newPos > entry.size) {
		newPos = entry.size;
	}
	ClusterNo clustersToRead = 1 + newPos / ClusterSize - cursor / ClusterSize;
	for (ClusterNo clusterIndex = 0; clusterIndex < clustersToRead; clusterIndex++) {
		int result = part->readCluster(fat->getCluster(cursorClusterFatEntry), loadedCluster);
		if (!result) {
			throw "Read from cluster failed";
		}
		BytesCnt startOffset = 0, endOffset = ClusterSize, destinationOffset;
		if (clusterIndex == 0) {
			startOffset = cursor % ClusterSize;
		}
		if (clusterIndex == clustersToRead - 1) {
			endOffset = newPos % ClusterSize;
		}
		destinationOffset = clusterIndex * ClusterSize - (cursor % ClusterSize) + startOffset;
		std::copy(loadedCluster + startOffset, loadedCluster + endOffset, buffer + destinationOffset);
		cursorClusterFatEntry = fat->getNextEntry(cursorClusterFatEntry);
	}
	BytesCnt bytesRead = newPos - cursor;
	cursor = newPos;
	cursorClusterIndex = cursor / ClusterSize;
	return bytesRead;
}

void KernelFile::seek (BytesCnt newPos) {
	if (newPos < 0 || newPos > entry.size) {
		throw "Seek position out of range";
	}
	cursor = newPos;
	if (cursorClusterIndex == newPos / ClusterSize) {
		return;
	}
	cursorClusterIndex = newPos / ClusterSize;
	cursorClusterFatEntry = entry.firstCluster;
	for (ClusterNo i = 0; i < cursorClusterIndex; i++) {
		cursorClusterFatEntry = fat->getNextEntry(cursorClusterFatEntry);
	}
}

BytesCnt KernelFile::filePos() {
	return cursor;
}

char KernelFile::eof () {
	return cursor == entry.size;
}

BytesCnt KernelFile::getFileSize () {
	return entry.size;
}

KernelFile::~KernelFile() {
	KernelFS::getInstance()->fileClosed(fileName);
}

KernelFile::KernelFile (Partition* part, FAT* fat, Entry entry, char mode, char* fileName) {
	this->part = part;
	this->fat = fat;
	this->entry = entry;
	this->mode = mode;
	if (mode == MODE_APPEND) {
		seek(entry.size);
	} else {
		seek(0);
	}
	this->fileName = fileName;
}