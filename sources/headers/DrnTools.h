#include <string.h>
<<<<<<< HEAD
#include <stdio.h>
=======
>>>>>>> e86d6612e0664a2b30f2f82d956ca36d10742bc9

unsigned int find_str (char *source, char *target, unsigned int sLen, unsigned int tLen, unsigned int sIndex)
{
	if (sLen == -1)
		sLen = strlen (source);
	if (tLen == -1)
		tLen = strlen (target);
		
	unsigned int fIndex = 0;
	for (; sIndex <= sLen - tLen + fIndex && fIndex != tLen; fIndex = source[sIndex] == target[fIndex] ? fIndex + 1 : 0, sIndex++);
	return fIndex == tLen ? sIndex - tLen : -1;
<<<<<<< HEAD
}

char *afterLast (char *source, char s)
{
	unsigned int fIndex = - 1, i = 0;
	for (; source [i] != '\0'; i++)
		if (source[i] == s)
			fIndex = i;

	return fIndex == -1 ? source : source + fIndex + 1;
}

char *readFile (FILE *fp)
{
	char *result = (char*)malloc (10240);
	unsigned int i = 0;

	while ((result[i++] = fgetc (fp)) != EOF && i < 10240);
	result[i - 1] = '\0';

	return result;
}

char *readFileFrStr (char *path)
{
	FILE *fp = fopen (path, "r");
	char *result = readFile (fp);

	fclose (fp);
	return result;
}
=======
}
>>>>>>> e86d6612e0664a2b30f2f82d956ca36d10742bc9
