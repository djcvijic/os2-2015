#include "path.h"
#include <cstring>

Path::Path(char* path) {
	if (strchr(path, ':') != NULL) {
		partition = path[0];
		path += 2;
	}
	pathString = new char[strlen(path) + 1];
	strcpy_s(pathString, strlen(path), path);
	if (!isBaseName()) {
		generateContainingDirectory();
		generateTopLevelDirectory();
	}
	generateBaseNameWOExtension();
}

bool Path::isAbsolute() {
	return pathString[0] == '\\';
}

bool Path::hasPartition() {
	return strchr(pathString, ':') != NULL;
}

bool Path::isBaseName() {
	return strchr(pathString, '\\') == NULL;
}

char* Path::toString() {
	char *result;
	if (hasPartition()) {
		result = new char[strlen(pathString) + 3];
		result[0] = partition;
		result[1] = ':';
		strcpy_s(result + 2, strlen(pathString), pathString);
	} else {
		result = new char[strlen(pathString) + 1];
		strcpy_s(result, strlen(pathString), pathString);
	}
	return result;
}

char Path::getPartition() {
	if (!hasPartition()) {
		throw "Path does not specify a partition";
	}
	return partition;
}

char* Path::getBaseName() {
	return strrchr(pathString, '\\') + 1;
}

char* Path::getBaseNameWOExtension() {
	return baseNameWOExtension;
}

char* Path::getExtension() {
	char *baseName = getBaseName();
	if (strrchr(baseName, '.') == NULL) {
		return NULL;
	}
	return strrchr(baseName, '.') + 1;
}

Path* Path::getContainingDirectory() {
	if (isBaseName()) {
		throw "Path does not specify a directory";
	}
	return new Path(containingDirectory);
}

char* Path::getTopLevelDirectory() {
	if (isBaseName()) {
		throw "Path does not specify a directory";
	}
	return topLevelDirectory;
}

Path* Path::getNextLevelPath() {
	if (isBaseName()) {
		throw "Path does not specify a directory";
	}
	return new Path(strchr(pathString, '\\') + 1);
}

Path::~Path() {
	delete []pathString;
	delete []baseNameWOExtension;
	if (!isBaseName()) {
		delete []containingDirectory;
		delete []topLevelDirectory;
	}
}

void Path::generateContainingDirectory() {
	containingDirectory = new char[strlen(pathString)];
	strcpy_s(containingDirectory, strlen(pathString), pathString);
	*(strrchr(containingDirectory, '\\')) = '\0';
}

void Path::generateBaseNameWOExtension() {
	char *baseName = getBaseName();
	baseNameWOExtension = new char[strlen(baseName)];
	strcpy_s(baseNameWOExtension, strlen(baseName), baseName);
	*(strrchr(baseNameWOExtension, '.')) = '\0';
}

void Path::generateTopLevelDirectory() {
	topLevelDirectory = new char[strchr(pathString, '\\') - pathString + 1];
	strcpy_s(topLevelDirectory, strchr(pathString, '\\') - pathString + 1, pathString);
	*(strchr(topLevelDirectory, '\\')) = '\0';
}