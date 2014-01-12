#pragma once

#include "list.h"

#define  PRG_NAME 	"Chatting Room"
#define  AUTHOR		"richard.xia"
#define  VERSION	"0.0.2"

#define  MSQ_KEY 	0x1234
#define  MAX_NAME  	20
#define  BUF_SIZE	255
/*
 *protocol:
  ---------------------------------------------------
 |CMD(1B):DST_ID(2B):TIME(4B):key(2B):MSG_SIZE(2B):MSG|
  ---------------------------------------------------
 MAX_SIZE: 255
 * */
#pragma pack(1)
struct msg_info {
	char cmd;
	unsigned short dst_id;
	time_t stime;
	unsigned short key;
	unsigned short msg_size;
	unsigned char content[0];
};
#pragma pack(0)

#define  CLI_KEY		0x1234
#define  SRV_KEY		0x4321
#define  CMD_SEP	':'

#define  CMD_LOGIN 	'l'
#define  CMD_LGOUT 	'Q'
#define  CMD_CHAT 	'C'
#define  CMD_BROD	'B'
#define  CMD_FILE 	'F'
#define  CMD_LIST	'L'

#define  SRV_ID		0x00

#define  USER_SEP	'#'

struct  msg_buf {
	long mtype;
	union {
		char text[BUF_SIZE];
		struct msg_info msg;
	} mbuf;
};

struct  user_info {
	int id;
	char name[MAX_NAME];
	struct list_head list;
};


