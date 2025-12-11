#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

  /* Native Windows, Cygwin, OS/2, DOS */
# define ISSLASH(C) ((C) == '/' || (C) == '\\')
  /* Internal macro: Tests whether a character is a drive letter.  */
# define HAS_DEVICE(Filename) \
    (isalpha((Filename)[0]) && (Filename)[1] == ':')
# define FILE_SYSTEM_PREFIX_LEN(Filename) (HAS_DEVICE (Filename) ? 2 : 0)
# ifdef __CYGWIN__
#  define FILE_SYSTEM_DRIVE_PREFIX_CAN_BE_RELATIVE 0
# else
   /* On native Windows, OS/2, DOS, the system has the notion of a
      "current directory" on each drive.  */
#  define FILE_SYSTEM_DRIVE_PREFIX_CAN_BE_RELATIVE 1
# endif
# if FILE_SYSTEM_DRIVE_PREFIX_CAN_BE_RELATIVE
#  define IS_ABSOLUTE_FILE_NAME(Filename) \
     ISSLASH ((Filename)[FILE_SYSTEM_PREFIX_LEN (Filename)])
# else
#  define IS_ABSOLUTE_FILE_NAME(Filename) \
     (ISSLASH ((Filename)[0]) || HAS_DEVICE (Filename))
# endif
# define IS_RELATIVE_FILE_NAME(Filename) \
    (! (ISSLASH ((Filename)[0]) || HAS_DEVICE (Filename)))
# define IS_FILE_NAME_WITH_DIR(Filename) \
    (strchr ((Filename), '/') != NULL || strchr ((Filename), '\\') != NULL \
     || HAS_DEVICE (Filename))

static size_t findFileBasename(const char *name, const char **basenamePtr)
{
	size_t nameLen = 0;
	size_t nextNameLen = 0;
	const char *ptr = NULL;
	const char *baseName = NULL;
	unsigned char charWasSlash = 0;
	size_t prefixLen = 0;

	/* check that name is not null and abort if it is.*/
	if(name == NULL)
	{
		return 0;
	}

	/* get the length of the name parameter and bounds check it. */
	nameLen = strlen(name);

	if(nameLen == 0)
	{
		return 0;
	}
	else if(nameLen > FILENAME_MAX)
	{
		return 0;
	}

	/* skip past <drive>: */
	prefixLen = FILE_SYSTEM_PREFIX_LEN(name);
	nameLen -= prefixLen;
	nextNameLen = nameLen;
	ptr = name + prefixLen;

	/* skip past initial / characters (root, double root, or network shares, or the / in <drive>:\ */
	for(; ptr != NULL && *ptr != '\0' && ISSLASH(*ptr); ptr++, nameLen--, nextNameLen--)
	{
	}
	baseName = ptr;

	/* move forward till just after the last / char. */
	for(; ptr != NULL && *ptr != '\0'; ptr++, nextNameLen--)
	{
		if(ISSLASH(*ptr))
		{
			charWasSlash = 1;
		}
		else if(charWasSlash)
		{
			charWasSlash = 0;
			baseName = ptr;
			nameLen = nextNameLen;
		}
	}

	/* return a pointer to the result if a place is provided to put it. */
	if(basenamePtr != NULL)
	{
		*basenamePtr = baseName;
	}
	return nameLen;
}

static size_t lengthWithoutEndSlashes(const char* baseName, size_t baseNameLen)
{
	size_t len = 0;
	size_t prefixLen = FILE_SYSTEM_PREFIX_LEN(baseName);

	/* change len by length of trailing slashes (remove trailing slashes), leaving the first / if all chars are / */
	for(len = baseNameLen; len > 1 && ISSLASH(baseName[len - 1]); len--)
	{
	}

	/* handle space case of //, the length will be 1 because we skipped all the slashes but the first. */
	if(len == 1 && ISSLASH(baseName[0]) && ISSLASH(baseName[1]) && ! baseName[2])
	{
		return 2;
	}
	else if(len == 1 && ISSLASH(baseName[0]) && ISSLASH(baseName[1]) && ISSLASH(baseName[2]) && !baseName[3])
	{
		return 2;
	}

	if(FILE_SYSTEM_DRIVE_PREFIX_CAN_BE_RELATIVE && prefixLen && len == prefixLen && ISSLASH(*baseName))
	{
		return prefixLen + 1;
	}

	return len;
}

size_t basename(const char *name, char *outname, size_t buflen)
{
	size_t expectedLen = 0;
	const char *baseName = NULL;
	unsigned char hasDotSlash = 0;

	if(name == NULL)
	{
		return 0;
	}

	expectedLen = findFileBasename(name, &baseName);

	if(*baseName != '\0')
	{
		const char * baseNamePtr;
		expectedLen = lengthWithoutEndSlashes(baseName, expectedLen);

		/* if it ends with slash, this may be a unix / path, so go ahead and save a / as this maybe a root path */
		expectedLen += ISSLASH(baseName[expectedLen]);

		/* crunch if they have a dotslash add */
		hasDotSlash = FILE_SYSTEM_PREFIX_LEN(baseName) ? 2 : 0;

		/* go ahead and strip off trailing slashes. */
		for(baseNamePtr = &baseName[expectedLen - 1]; expectedLen > 1 && ISSLASH(*baseNamePtr); expectedLen--, baseNamePtr--)
		{
		}
	}
	else
	{
		expectedLen = lengthWithoutEndSlashes(name, strlen(name));
		baseName = name;
		hasDotSlash = 0;
	}
	expectedLen += hasDotSlash;

	if(buflen < expectedLen + 1)
	{
		return expectedLen + 1;
	}
	else if(outname == NULL)
	{
		return expectedLen + 1;
	}
	else
	{
		memset(outname, '\0', buflen);
		if(hasDotSlash)
		{
			outname[0] = '.';
			outname[1] = '/';
			strncpy(outname + 2, baseName, expectedLen - 2);
		}
		else
		{
			strncpy(outname, baseName, expectedLen);
		}
	}

	return expectedLen + 1;
}

size_t dirname(const char *name, char *outname, size_t buflen)
{
	size_t rootDirNameLen = 0;
	size_t length = 0;
	const char * ptr;

	if(name == NULL)
	{
		return 0;
	}

	rootDirNameLen = FILE_SYSTEM_PREFIX_LEN(name);
	if(rootDirNameLen != 0)
	{
		/* if the file system directory is relative to each drive and the drive has a slash in it (aka c:\*) then add the slash */
		if(FILE_SYSTEM_DRIVE_PREFIX_CAN_BE_RELATIVE && ISSLASH (name[rootDirNameLen]))
		{
			rootDirNameLen += 1;
		}
	}
	else
	{
		/* if the path starts with a / or a \ then it's either a linux style abolute path or a network share */
		if(ISSLASH (name[0]))
		{
			/* if it's a network share (// is distinct root) and it begns with 2 slashes then add then. */
			if(ISSLASH (name[1]) && !ISSLASH(name[2]))
			{
				rootDirNameLen += 2;
			}
			/* else just add 1 for the root / */
			else
			{
				rootDirNameLen += 1;
			}
		}
	}

	/* goto the last slash and step bardwards till you get before all of the slashes. */
	length = findFileBasename(name, &ptr);
	if(ptr == NULL)
	{
		return 0;
	}

	for(length = ptr - name - 1; length > rootDirNameLen && ISSLASH(*ptr); length--, ptr--)
	{
	}

	if(buflen < length + 1)
	{
		return length + 1;
	}
	else if(outname == NULL)
	{
		return length + 1;
	}
	else
	{
		memset(outname, '\0', buflen);
		strncpy(outname, name, length);
	}

	return length + 1;
}

