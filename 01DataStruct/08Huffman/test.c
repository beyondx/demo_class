#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huff.h"

int main(int argc, char *argv[])
{
	if (argc < 3) {
		ERR_PRINTF("Miss args\n");
		printf("Usage: %s [-c|-d] src dest\n", argv[0]);
		return -1;
	}
	
	struct huff_info *hf = (struct huff_info *)malloc(sizeof(*hf));
	
	huff_init(hf);

	if (strcmp(argv[1], "-c") == 0) {
		hf->compress(hf, argv[2], argv[3]);
	} else if(strcmp(argv[1], "-d") == 0) {
		hf->decompress(hf, argv[2], argv[3]);
	} else {
		ERR_PRINTF("Unkown arg '%s'\n", argv[1]);
	}

	huff_destroy(hf);
	free(hf);
		
	return 0;
}
