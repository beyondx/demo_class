#include <stdio.h>

int main()
{
	char rbuf = 0;

	FILE *pf = fopen("/etc/passwd", "r");

	FILE *pfnull = fopen("/dev/null", "rw");

	if (pf == NULL) {
		perror("fopen");
		goto err;
	}

	while (fread(&rbuf, 1, 1, pf) != 0) {
		fwrite(&rbuf, 1, 1, pfnull);
	}

	fclose(pf);
	return 0;
	
err:
	return -1;
}
