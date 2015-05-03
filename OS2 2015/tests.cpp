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
	std::cin >> configFile;
	Partition *partition = new Partition(configFile);
	std::cout << "Size of partition is: " << partition->getNumOfClusters() << '\n';
	std::cout << "Enter data to write to first cluster: \n";
	char clusterData[3000];
	std::cin >> clusterData;
	char result = partition->writeCluster(0, (char*)clusterData);
	std::cout << "Result = " << (int)result << '\n';
	std::cout << "Enter data to write to second cluster: \n";
	std::cin >> clusterData;
	result = partition->writeCluster(1, (char*)clusterData);
	std::cout << "Result = " << (int)result << '\n';
	result = partition->readCluster(0, (char*)clusterData);
	clusterData[2048] = 0;
	std::cout << "Data read from first cluster: \n" << clusterData << '\n';
	result = partition->readCluster(1, (char*)clusterData);
	clusterData[2048] = 0;
	std::cout << "Data read from second cluster: \n" << clusterData << '\n';
	std::cout << "Done testing Partition!\n";
}

void testPath() {
	std::cout << "Testing Path...\nEnter an absolute path:\n";
	char sAbsolutePath[256];
	std::cin >> sAbsolutePath;
	Path *absolutePath = new Path(sAbsolutePath);
}

void testFAT() {
}

void testDirectory() {
}

void testKernelFile() {
}

void testKernelFS() {
}

/*int main(int argc, char** argv) {
	std::cout << "Tests starting...\n";
	try {
		testPart();
		//testPath();
		//testFAT();
		//testDirectory();
		//testKernelFile();
		//testKernelFS();
	} catch (const char* e) {
		std::cout << "Tests failed: " << e << '\n';
	}
}*/