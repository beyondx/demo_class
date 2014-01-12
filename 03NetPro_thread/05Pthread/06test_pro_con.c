#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define DATA_MAX 5
#define PRO_INIT_VAL (DATA_MAX / 2)
#define CON_INIT_VAL (DATA_MAX - PRO_INIT_VAL)
static int data; //仓库
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex; //读写互斥锁，无论生产者还是消费者，在访问data的时候，本锁都为0
static sem_t sem_pro; //生产者计数锁，为0时生产者挂起
static sem_t sem_con; //消费者计数锁，为0时消费者挂起

static void *pro_handler(void *arg)
{
	ssize_t i = 100;

	for (; i > 0; --i) {
		//sem_wait(&sem_pro);
		pthread_mutex_lock(&mutex);

		++data;
		printf("producter: data = %d\n", data);

		pthread_mutex_unlock(&mutex);
		//sem_post(&sem_con);
		pthread_cond_signal(&cond);
		usleep(100000);
	}

}

static void *con_handler(void *arg)
{
	ssize_t i = 100;

//	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

	for (; i > 0; --i) {
		//sem_wait(&sem_con);
		pthread_mutex_lock(&mutex);
		while (data == 0) {
			pthread_cond_wait(&cond, &mutex);
		}
		while (data > 0) {
			--data;
			printf("consumer: data = %d\n", data);
		}
		pthread_mutex_unlock(&mutex);
		//sem_post(&sem_pro);
		sleep(1);
	}
}

int main()
{
	pthread_mutex_init(&mutex, NULL);

	sem_init(&sem_pro, 0, PRO_INIT_VAL);
	sem_init(&sem_con, 0, CON_INIT_VAL);

	data = CON_INIT_VAL;

	pthread_t pro_id, con_id;
	
	int pro_ret = 0;
	if (pro_ret = pthread_create(&pro_id, NULL, pro_handler, NULL)) {
		fprintf(stderr, "pthread_create producter: %s", strerror(pro_ret));
		goto err_create_producter;
	}

	int con_ret = 0;
	if (con_ret = pthread_create(&con_id, NULL, con_handler, NULL)) {
		fprintf(stderr, "pthread_create consumer: %s", strerror(con_ret));
		goto err_create_consumer;
	}
#if 0
	sleep(3);
	if (pthread_cancel(con_id)) {
		fprintf(stderr, "error cancel\n");
	}
#endif
	pthread_join(pro_id, NULL);
	pthread_join(con_id, NULL);

	sem_destroy(&sem_con);
	sem_destroy(&sem_pro);
	pthread_mutex_destroy(&mutex);
	return 0;

	//pthread_join(con_id, NULL);
err_create_consumer:
	pthread_join(pro_id, NULL);
err_create_producter:
	sem_destroy(&sem_con);
	sem_destroy(&sem_pro);
	pthread_mutex_destroy(&mutex);
	return -1;
}
