#include <stdio.h>
#include <unistd.h>

int main()
{
        int i = 2;
        int j = 3;
        int t = 0;
        int r = 0;

        printf("frist: pid = %d, ppid = %d\n", getpid(), getppid());
        for (; t < i; ++t) {
                r = fork();
                if (r < 0)
                        exit(0);
                else if (r) // (r)
                        exit(0);
        }
        printf("pid = %d, ppid = %d, after i\n", getpid(), getppid());
  	for (; t < j; ++t) {
                r = fork();
                if (r < 0)
                        exit(0);
                else if (!r) // (r)
                        exit(0);
        }
        printf("pid = %d, ppid = %d, after j\n", getpid(), getppid());
        usleep(1000);
        return 0;
}
