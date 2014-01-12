#pragma once
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>



#define  DBG_PRINT(level, fmt, args...) \
	fprintf(stderr, __FILE__ ":%s:%d:" #level":"fmt"", __FUNCTION__, __LINE__, ##args)

#define LOG_PRINT(fmt, args...)	\
		DBG_PRINT([LOG], fmt, ##args)

#define  err_ret(msg) \
	do { perror("[Error]:"msg);} while (0)

#define ERR_PRINT(fmt, args...) \
		do {DBG_PRINT([ERROR], fmt, ##args);}while(0)

#define ALERT_PRINT(fmt, args...) \
	do { DBG_PRINT([ALERT], fmt, ##args); exit(EXIT_FAILURE);} while (0)

#define MAX 100


struct term_t {
	int fd_dev;
	struct termios *set;
	int (*open_dev)(struct term_t *info, const char *dev_name);
	int (*set_speed)(struct term_t *info, int speed);
	int (*set_attr)(struct term_t *info, int data_bit, int stop_bit, int perm);
	int (*read_dev)(struct term_t *info, void *buf, int len);
	int (*write_dev)(struct term_t *info, const void *buf, int len);
	int (*read_nbytes)(struct term_t *info, char *buf, int len);
	int (*write_nbytes)(struct term_t *info, const char *buf, int len);
	int (*read_line)(struct term_t *info, char *buf, int max);
};

void term_init(struct term_t *info);
void term_destroy(struct term_t *info);
