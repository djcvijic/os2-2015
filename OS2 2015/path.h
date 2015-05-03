#pragma once

class Path {
public:
	Path(char* path);
	bool isAbsolute();
	bool isBaseName();
	bool hasPartition();
	char* toString();
	char getPartition();
	char* getBaseName();
	char* getBaseNameWOExtension();
	char* getExtension();
	Path* getContainingDirectory();
	char* getTopLevelDirectory();
	Path* getNextLevelPath();
	~Path();
private:
	char partition;
	char *pathString;
	char *containingDirectory;
	char *baseNameWOExtension;
	char *topLevelDirectory;

	void generateContainingDirectory();
	void generateBaseNameWOExtension();
	void generateTopLevelDirectory();
};