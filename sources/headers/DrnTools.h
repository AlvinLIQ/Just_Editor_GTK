#include <string.h>

unsigned int find_str (char *source, char *target, unsigned int sLen, unsigned int tLen, unsigned int sIndex)
{
	if (sLen == -1)
		sLen = strlen (source);
	if (tLen == -1)
		tLen = strlen (target);
		
	unsigned int fIndex = 0;
	for (; sIndex <= sLen - tLen + fIndex && fIndex != tLen; fIndex = source[sIndex] == target[fIndex] ? fIndex + 1 : 0, sIndex++);
	return fIndex == tLen ? sIndex - tLen : -1;
}