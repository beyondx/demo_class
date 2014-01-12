#include <stdio.h>
#include <assert.h>

#define  BUF_SIZE	4096

int main(int argc, char **argv)
{
	assert(argc >= 2);	
	
	FILE *pf_ro = fopen(argv[1], "r");
	
	if (pf_ro == NULL) {
		perror("fopen failed");
		goto err_fopen;
	}

	unsigned char rbuf[4096] = {0};

	fseek(pf_ro, 100, SEEK_SET);
	size_t n;
	while ((n = fread(rbuf, 1, BUF_SIZE, pf_ro)) != 0) {
		fwrite(rbuf, 1, n, stdout);
	}

	fclose(pf_ro);

	return 0;
err_fopen:
	return -1;
}
