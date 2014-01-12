#include <stdio.h>
#include <unistd.h>

int main()
{
	setbuf(stdout, NULL);
	while (1) {
		sleep(2);
		fprintf(stdout, "This is From stdout.");
		//fflush(stdout);
		fprintf(stderr, "this is stderr.");
	}

	return 0;
}
