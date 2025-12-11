#include <stdio.h>

#include "common.h"

int main(int argc, char **argv) {
	size_t len=0;
	char baseName[1024] = {0};
	char dirName[1024] = {0};
	if(argc != 2)
	{
		fprintf(stderr, "incorrect nubmer of args, expected: %s <path>\n", argv[0]);
		return 1;
	}
	fprintf(stderr, "baseName: \"%s\"\n", argv[1]);
	len = basename(argv[1], baseName, sizeof baseName);
	fprintf(stdout, "basename: length: %lu, value: \"%s\"\n", (unsigned long)len, baseName);
	len = dirname(argv[1], dirName, sizeof dirName);
	fprintf(stdout, "dirname: length: %lu, value: \"%s\"\n", (unsigned long)len, dirName);

	return 0;
}

