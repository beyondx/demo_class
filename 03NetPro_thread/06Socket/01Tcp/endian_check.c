#include <stdio.h>
#include <arpa/inet.h>

void endian_check(void)
{
	unsigned int  ia = 0x12345678;
	unsigned char  cb = ia & 0xff;

	if (cb == 0x78) {
		printf("little endian\n");
	} else if (cb == 0x12) {
		printf("big endian\n");
	} else {
		fprintf(stderr, "Unkown endian\n");
	}
}

int main(int argc, char *argv[])
{
	unsigned  int ia = 0x12345678;

	printf("host:%#x\n", ia);

	printf("network:%#x\n", htonl(ia));

	return 0;
}
