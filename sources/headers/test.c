#include "DrnTools.h"
#include <stdio.h>

int main (int argc, char** argv)
{
	printf ("%d\n", find_str (argv[1], argv[2], -1, -1, 0));
	return 0;
}