#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "term.h"

#define NDEBUG
#include <assert.h>
int speed_arr[] =  { B230400, B115200, B57600, B38400, B19200, B9600, B4800};
int name_arr[]  =  {  230400,  115200,  57600,  38400,  19200,  9600,  4800};

//	int fd_dev;
//	struct termios *set;
/* open device,  set fd_dev*/
static int term_open_dev(struct term_t *info, const char *dev_name)
{
	assert(info != NULL && dev_name != NULL);
	info->fd_dev = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);

	assert(info->fd_dev != -1);
	if (info->fd_dev == -1) {
		ERR_PRINT("open device %s failed", dev_name);
		perror("");
		goto err_open_device;
	}

	return 0;

err_open_device:
	return -1;
}

static int term_set_speed(struct term_t *info, int speed)
{
	int i;
	int status;
	tcgetattr(info->fd_dev, info->set); //get default set

	/*get constant key by int speed value*/
	for (i = 0; i < sizeof(speed_arr) / sizeof(int); ++i) {
		if (speed != name_arr[i]) {
			continue;
		}else {
			//flush buffer, avoid dirty buffer
			tcflush(info->fd_dev, TCIOFLUSH);
			//set speed in & out
			cfsetispeed(info->set, speed_arr[i]);
			cfsetospeed(info->set, speed_arr[i]);
			status = tcsetattr(info->fd_dev, TCSANOW, info->set);
			if (status != 0) {
				ERR_PRINT("tcsetattr failed, status = %d ", status);
				perror("");
				goto err_tcsetattr;
			}
			break;
		}
	}
	LOG_PRINT("speed = %d\n", name_arr[i]);
	#if 1	
	//speed out range
	if (sizeof(speed_arr) / sizeof(speed_arr[0]) == i) {
		fprintf(stderr,"out of range[4800~230400]\n");
		printf("[Note]: 230400,  115200,  57600,  38400,  19200,  9600,  4800\n");
		goto err_speed_out_range;
	}
	#endif
	//clear buffer
	tcflush(info->fd_dev, TCIOFLUSH);

	return 0;

err_speed_out_range:
err_tcsetattr:
	return -1;
}

/* set data_bit stop_bit parity[奇偶校验位]*/
static int term_set_attr(struct term_t *info, int data_bit,
				int stop_bit, int parity)
{
	//assert 
	assert(info != NULL);

	//get attr
	if (tcgetattr(info->fd_dev, info->set) != 0) {
		ERR_PRINT("tcgetattr failed");
		perror("");
		goto err_get_attr;
	}
	//cflag clear
	info->set->c_cflag &= ~CSIZE;
	// set data_bit
	switch(data_bit) {
		case 7:
			info->set->c_cflag |= CS7;
			break;
		case 8:
			info->set->c_cflag |= CS8;
			break;
		defalut:
			ALERT_PRINT("unsupported data_bit: %d", data_bit);
			goto err_data_bit;
	}
	//set stop bit
	switch (stop_bit) {
		case 1:
			info->set->c_cflag &= ~CSTOPB;//1bit, disable
			break;
		case 2:
			info->set->c_cflag |= CSTOPB; //2bit, enable
			break;
		default:
			ALERT_PRINT("unspported stop_bit\n");
			goto err_stop_bit;
	}

	//set parity
	switch (parity) {
		case 'n':
		case 'N':
			info->set->c_cflag &= ~PARENB; //clear parity
			info->set->c_iflag &= ~INPCK; // disable parity check
			break;
		case 'o': // odd 奇校验
		case 'O':
			info->set->c_cflag &= (PARODD |PARENB); // odd
			info->set->c_iflag |= INPCK; // enalbe parity check;
			break;
		case 'e': // unodd 偶校验
		case 'E':
			info->set->c_cflag &= ~PARODD; // disable odd
			info->set->c_cflag |= PARENB; // odd
			info->set->c_iflag |= INPCK; // enalbe parity check;
			break;
		default:
			ERR_PRINT("unsupport parity");
			fprintf(stderr,"choice:[n|o|e]\n");
			goto err_parity;
	}

	// disalbe  flow contorl //XXX FIXME
	info->set->c_iflag &= ~(IXON | IXOFF | IXANY);
	info->set->c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG | CRTSCTS);
	info->set->c_oflag &= ~OPOST;
	
	//timeout 
	info->set->c_cc[VTIME] = 150;
	info->set->c_cc[VMIN] = 0;

	tcflush(info->fd_dev, TCIOFLUSH);

	if (tcsetattr(info->fd_dev, TCSANOW, info->set) != 0) {
		ERR_PRINT("set attr failed");
		goto err_set_attr;
	}

	return 0;
err:
err_set_attr:
err_stop_bit:
err_parity:
err_data_bit:
err_get_attr:
	return -1;
}
//考虑信号中断阻塞
static	int term_read_dev(struct term_t *info, void *buf, int len)
{
	assert(buf != NULL);
	assert(info != NULL);
	int n = 0;	
	while ((n = read(info->fd_dev, buf, len)) < 0) {
		if (errno != EINTR) {
			goto err;
			//break;
		}
	}

	return n;

err:
	return -1;

}

static int term_write_dev(struct term_t *info, const void *buf, int len)
{	
	assert(buf != NULL);
	assert(info != NULL);
	int n;
	while ((n = write(info->fd_dev, buf, len)) < 0) {
		if (errno == EINTR) {
			continue;	
		} else {
			goto err;
		}
	}
	if (n == len) {
		return n;
	} else {
		tcflush(info->fd_dev, TCOFLUSH );
		return -1;
	}
err:
	return -1;
}
//基于缓冲区,攒够len大小再返回
static	int term_read_nbytes(struct term_t *info, char *buf, int len)
{
	int count = 0;
	int nleft = len;
	char *ptr = buf;

	while (nleft > 0) 
	{
		if ((count = term_read_dev(info, ptr, nleft)) < 0) {
			goto err;
		}else if (count == 0){
			break;
		}
		ptr += count;	
		nleft -= count;
	}

	return len - nleft;
err:
	return count;
}


//  FIXME 
static int term_write_nbytes(struct term_t *info, const char *buf, int len)
{
	int count = 0;
	int nleft = len;
	char *ptr = buf;

	while (nleft > 0) 
	{
		if ((count = term_write_dev(info, ptr, nleft)) < 0) {
			goto err;
		}else if (count == 0){
			break;
		}
		ptr += count;	
		nleft -= count;
	}

	return len - nleft;
err:
	return -1;

}
//采集一行内容
static int term_read_line(struct term_t *info, char *buf, int max)
{
	int count = 0;
	unsigned char tmp = 0;
	do {
		if (term_read_dev(info, &tmp, 1) < 0) {
			goto err;
		}
		
		buf[count] = tmp;
		if (tmp == '\n' || tmp == '\r') {
			break;
		}
		count += 1;
	}while (count != max);
	
	return 0;
err:
	return -1;

}

void term_init(struct term_t *info)
{
	info->fd_dev = -1;

	info->set = (struct termios *)malloc(sizeof(struct termios));
	assert(info->set != NULL);

	if (info->set == NULL) {
		ALERT_PRINT("malloc failed,out of memory\n");
	}

	info->open_dev = term_open_dev;
	info->set_speed = term_set_speed;
	info->set_attr = term_set_attr;
	info->read_dev = term_read_dev;
	info->write_dev = term_write_dev;
	info->read_nbytes = term_read_nbytes;
	info->write_nbytes = term_write_nbytes;
	info->read_line = term_read_line;

	LOG_PRINT("term_init\n");
}

void term_destroy(struct term_t *info)
{
	free(info->set);
	LOG_PRINT("term_destroy\n");
}

