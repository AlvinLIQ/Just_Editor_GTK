#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define bool unsigned
#define true 1
#define false 0
#define uchar unsigned char
#define uint unsigned int

#define writeFile(fp, wText) fprintf(fp, "%s", wText)

uint find_str (char *source, char *target, uint sLen, uint tLen, uint sIndex)
{
	if (sLen == -1)
		sLen = strlen (source);
	if (tLen == -1)
		tLen = strlen (target);
		
	uint fIndex = 0;
	for (; sIndex <= sLen - tLen + fIndex && fIndex != tLen; fIndex = source[sIndex] == target[fIndex] ? fIndex + 1 : 0, sIndex++);
	return fIndex == tLen ? sIndex - tLen : -1;
}

char *afterLast (char *source, char s)
{
	uint fIndex = - 1, i = 0;
	for (; source [i] != '\0'; i++)
		if (source[i] == s)
			fIndex = i;

	return fIndex == -1 ? source : source + fIndex + 1;
}
/*
void writeFile (FILE *fp, char *wText)
{
	fprintf (fp, "%s", wText);
}
*/
void writeFileFrStr (char *path, char *wText)
{
	FILE *fp = fopen (path, "wb");
	writeFile (fp, wText);

	fclose (fp);
}

char *readFile (FILE *fp)
{
	uchar *result = (char*)malloc (40960);
	uint i = 0;

	while ((result[i++] = fgetc (fp)) != (uchar)EOF && i < 40960);

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
