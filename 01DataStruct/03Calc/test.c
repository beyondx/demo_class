#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

void usage(const char *prg_name)
{
	printf("%s \"expr\"\n", prg_name);
}


int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "miss args\n");
		usage(argv[0]);
		exit(1);
	}
	
	struct calc_info  *q =  (struct calc_info *)malloc(sizeof(*q));

	if (q == NULL) {
		perror("malloc");
		goto err_malloc;
	}

	calc_init(q);
	num_t ret = 0;

	int i;
	for (i = 1; i < argc; ++i) {
		ret = q->calc(q, argv[i]);
		printf("\n---%s = %d----\n", argv[i], ret);
	}

	calc_destroy(q);
	free(q);

	return 0;
err_malloc:
	return -1;
}
