#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>

//#define   BIG_FILE_NAME "/var/ftp/pub/software/rhel-server-5.5-i386-dvd.iso"
//#define   BIG_FILE_NAME "/var/ftp/pub/software/星际译王/词库/other/WyabdcRealPeopleTTS-1.0-1.noarch.rpm"
//#define   BIG_FILE_NAME "/var/ftp/pub/book/ubuntu/Ubuntu权威指南.刑国庆.扫描版.pdf"
#define   BIG_FILE_NAME "./1G"

#ifndef   BUF_SIZE
#define   BUF_SIZE	4096
#endif
#define   DEFAULT_THR_NUM	3

char rbuf[BUF_SIZE];
int fd;
int total_size;

struct  thr_info {
	pthread_t tid;
	size_t  nread;
};

struct thr_info *pthr;

static pthread_mutex_t  rmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t   cond = PTHREAD_COND_INITIALIZER;


/*验证多线程对单一大文件的读取的效率*/
void *thr_read_file(void *arg)
{
	ssize_t nread;
	int ret;
	int idx = (int )arg;
	printf("pthread_%d running\n", idx);
	pthread_t tid = pthread_self();
	
	for (;;) {
		pthread_mutex_lock(&rmutex);
		re_try:
		nread = read(fd, rbuf, BUF_SIZE);
		if (nread < 0) {
			if (errno == EINTR) {
				goto re_try;
			}else {
				perror("read");
				goto err;
			}
		} else if (nread == 0) {
			pthr[idx].nread = (size_t)nread;
			break;
		} else {
			pthr[idx].nread = (size_t)nread;
		}
		total_size += nread;

		pthread_mutex_unlock(&rmutex);
		ret = pthread_cond_signal(&cond);
		if (ret != 0) {
			perror("pthread_cond_signal");
			goto err;
		}
	}	

	pthread_mutex_unlock(&rmutex);

	return (void *)0;
err:
	pthread_mutex_unlock(&rmutex);
	return (void *)-1;	
}

int main(int argc, char *argv[])
{
	printf("process is start, pid = %d\n", getpid());
	int n_thrs; 	
	if (argc < 2) {
		n_thrs = DEFAULT_THR_NUM;
	} else  {
		n_thrs = atoi(argv[1]);
	}
	assert(n_thrs > 0);

	//打开目标文件
	fd = open(BIG_FILE_NAME, O_RDONLY);
	
	if (fd < 0) {
		perror("open");
		exit(-1);
	}
	struct timeval time_start, time_end;
	gettimeofday(&time_start, NULL);

	pthr = (struct thr_info *)calloc(n_thrs, sizeof(*pthr));

	int idx, ret;
	for ( idx = 0; idx < n_thrs; ++idx) {
		ret = pthread_create(&pthr[idx].tid, NULL, thr_read_file, (void *)idx);
		if (ret != 0) {
			perror("pthread_create");
			exit(-1);
		}
	}

	for (idx = 0; idx < n_thrs; ++idx) {
		ret = pthread_join(pthr[idx].tid, NULL);
		if (ret != 0) {
			perror("pthread_join");
			exit(-1);
		}
	}
	gettimeofday(&time_end, NULL);
	printf("total_size:%lu, cost: %lums\n", total_size, (time_end.tv_usec - time_start.tv_usec) / 1000);
	
	return 0;
}
