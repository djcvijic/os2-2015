#pragma once

#include <cstring>

class Path {
public:
	Path(char* path, char part = '\0');
	bool isAbsolute();
	bool isBaseName();
	bool hasPartition();
	char* toString();
	char getPartition();
	char* getBaseName();
	char* getBaseNameWOExtension();
	char* getExtension();
	Path* getContainingDirectoryPath();
	char* getTopLevelDirectory();
	Path* getNextLevelPath();
	~Path();
private:
	char partition;
	char *pathString;
	char *containingDirectory = NULL;
	char *baseNameWOExtension = NULL;
	char *topLevelDirectory = NULL;

	void generateContainingDirectory();
	void generateBaseNameWOExtension();
	void generateTopLevelDirectory();
};