#include <stdio.h>
#include <syslog.h>

#define  PRG_NAME	"uplookingd"

int main(int argc, char *argv[])
{
	printf("this is a syslog demo\n");
	
	openlog(PRG_NAME, LOG_PID, LOG_SYSLOG);

	syslog(LOG_ERR| LOG_SYSLOG, "this is from uplookingd, pid = %d\n", getpid());
	
	return 0;
}
