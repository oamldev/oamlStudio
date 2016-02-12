#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oamlCommon.h"


static std::string absPath = "";

static void* oamlOpen(const char *filename) {
	std::string fullpath(absPath + filename);
	return fopen(fullpath.c_str(), "rb");
}

static size_t oamlRead(void *ptr, size_t size, size_t nitems, void *fd) {
	return fread(ptr, size, nitems, (FILE*)fd);
}

static int oamlSeek(void *fd, long offset, int whence) {
	return fseek((FILE*)fd, offset, whence);
}

static long oamlTell(void *fd) {
	return ftell((FILE*)fd);
}

static int oamlClose(void *fd) {
	return fclose((FILE*)fd);
}


oamlFileCallbacks studioCbs = {
	&oamlOpen,
	&oamlRead,
	&oamlSeek,
	&oamlTell,
	&oamlClose
};

void InitCallbacks(std::string prjPath) {
	absPath = prjPath;
}
