#include "part.h"
#include "path.h"
#include "fat.h"
#include "directory.h"
#include "kernelfile.h"
#include "kernelfs.h"
#include "file.h"
#include "fs.h"
#include <iostream>

void testPart() {
	std::cout << "Testing Partition...\nEnter the config file: ";
	char configFile[256];
	std::cin.getline(configFile, 250);
	Partition *partition = new Partition(configFile);
	std::cout << "Size of partition is: " << partition->getNumOfClusters() << '\n';
	std::cout << "Enter data to write to first cluster: \n";
	char clusterData[3000];
	std::cin.getline(clusterData, 3000);
	char result = partition->writeCluster(0, (char*)clusterData);
	std::cout << "Result = " << (int)result << '\n';
	std::cout << "Enter data to write to second cluster: \n";
	std::cin.getline(clusterData, 3000);
	result = partition->writeCluster(1, (char*)clusterData);
	std::cout << "Result = " << (int)result << '\n';
	char outputData[3000];
	result = partition->readCluster(0, (char*)outputData);
	outputData[2999] = 0;
	std::cout << "Data read from first cluster: \n" << outputData << '\n';
	result = partition->readCluster(1, (char*)outputData);
	outputData[2999] = 0;
	std::cout << "Data read from second cluster: \n" << outputData << '\n';
	std::cout << "Done testing Partition!\n";
}

void testPath() {
	std::cout << "Testing Path...\nEnter a path:\n";
	char sPath[256];
	std::cin.getline(sPath, 256);
	Path *path = new Path(sPath);
	std::cout << "isAbsolute: " << path->isAbsolute() << '\n';
	std::cout << "isBaseName: " << path->isBaseName() << '\n';
	std::cout << "hasPartition: " << path->hasPartition() << '\n';
	if (path->hasPartition()) {
		std::cout << "getPartition: " << path->getPartition() << '\n';
	}
	std::cout << "getBaseName: " << path->getBaseName() << '\n';
	std::cout << "getBaseNameWOExtension: " << path->getBaseNameWOExtension() << '\n';
	std::cout << "getExtension: " << path->getExtension() << '\n';
	if (!path->isBaseName()) {
		std::cout << "getTopLevelDirectory: " << path->getTopLevelDirectory() << '\n';
		std::cout << "getContainingDirectory: " << path->getContainingDirectoryPath()->toString() << '\n';
		std::cout << "getNextLevelPath: " << path->getNextLevelPath()->toString() << '\n';
	}
	std::cout << "Done testing Path!\n";
}

void testFAT() {
}

void testDirectory() {
}

void testKernelFile() {
}

void testKernelFS() {
}

int main(int argc, char** argv) {
	std::cout << "Tests starting...\n";
	try {
		//testPart();
		//testPath();
		//testFAT();
		//testDirectory();
		//testKernelFile();
		//testKernelFS();
	} catch (const char* e) {
		std::cout << "Tests failed: " << e << '\n';
	}
}