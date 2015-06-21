#include "path.h"
#include <cstring>

Path::Path(char* path, char part) {
	partition = part;
	if ((partition == '\0') && (strchr(path, ':') != NULL)) {
		partition = path[0];
		path += 2;
	}
	pathString = new char[strlen(path) + 1];
	strcpy_s(pathString, strlen(path) + 1, path);
}

bool Path::isAbsolute() {
	return pathString[0] == '\\';
}

bool Path::hasPartition() {
	return partition != '\0';
}

bool Path::isBaseName() {
	return strchr(pathString, '\\') == NULL;
}

char* Path::toString() {
	char *result;
	if (hasPartition() && isAbsolute()) {
		result = new char[strlen(pathString) + 3];
		result[0] = partition;
		result[1] = ':';
		strcpy_s(result + 2, strlen(pathString) + 1, pathString);
	} else {
		result = new char[strlen(pathString) + 1];
		strcpy_s(result, strlen(pathString) + 1, pathString);
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
	char* lastBackslash = strrchr(pathString, '\\');
	if (lastBackslash == NULL) {
		return pathString;
	}
	return strrchr(pathString, '\\') + 1;
}

char* Path::getBaseNameWOExtension() {
	if (baseNameWOExtension == NULL) {
		generateBaseNameWOExtension();
	}
	return baseNameWOExtension;
}

char* Path::getExtension() {
	char *baseName = getBaseName();
	if (strrchr(baseName, '.') == NULL) {
		return "";
	}
	return strrchr(baseName, '.') + 1;
}

Path* Path::getContainingDirectoryPath() {
	if (isBaseName()) {
		throw "Path does not specify a directory";
	}
	if (containingDirectory == NULL) {
		generateContainingDirectory();
	}
	return new Path(containingDirectory, partition);
}

char* Path::getTopLevelDirectory() {
	if (isBaseName()) {
		throw "Path does not specify a directory";
	}
	if (topLevelDirectory == NULL) {
		generateTopLevelDirectory();
	}
	return topLevelDirectory;
}

Path* Path::getNextLevelPath() {
	if (isBaseName()) {
		throw "Path does not specify a directory";
	}
	return new Path(strchr(pathString, '\\') + 1, partition);
}

Path::~Path() {
	delete []pathString;
	if (baseNameWOExtension != NULL) {
		delete []baseNameWOExtension;
	}
	if (containingDirectory != NULL) {
		delete []containingDirectory;
	}
	if (topLevelDirectory != NULL) {
		delete []topLevelDirectory;
	}
}

void Path::generateContainingDirectory() {
	containingDirectory = new char[strlen(pathString) + 1];
	strcpy_s(containingDirectory, strlen(pathString) + 1, pathString);
	char* lastBackslash = strrchr(containingDirectory, '\\');
	if (lastBackslash != NULL) {
		*lastBackslash = '\0';
	}
}

void Path::generateBaseNameWOExtension() {
	char *baseName = getBaseName();
	baseNameWOExtension = new char[strlen(baseName) + 1];
	strcpy_s(baseNameWOExtension, strlen(baseName) + 1, baseName);
	char* lastDot = strrchr(baseNameWOExtension, '.');
	if (lastDot != NULL) {
		*lastDot = '\0';
	}
}

void Path::generateTopLevelDirectory() {
	int tldLength = strchr(pathString, '\\') - pathString + 1;
	topLevelDirectory = new char[tldLength];
	memcpy(topLevelDirectory, pathString, tldLength);
	char* firstBackslash = strchr(topLevelDirectory, '\\');
	if (firstBackslash != NULL) {
		*firstBackslash = '\0';
	}
}
