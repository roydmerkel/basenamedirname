#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

size_t basename(const char *name, char *outname, size_t buflen);
size_t dirname(const char *name, char *outname, size_t buflen);

#endif /* #ifndef COMMON_H */
