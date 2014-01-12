#include <stdio.h>
#include <stdlib.h>

int main()
{
	printf("%ld\n", strtol("123:abc\n", NULL, 10));

	return 0;
}
