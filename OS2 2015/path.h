#pragma once

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
	char *containingDirectory = 0;
	char *baseNameWOExtension = 0;
	char *topLevelDirectory = 0;

	void generateContainingDirectory();
	void generateBaseNameWOExtension();
	void generateTopLevelDirectory();
};