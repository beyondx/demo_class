#pragma once

#include "list.h"

#define  PRG_NAME "Chatting Room"
#define  AUTHOR "richard.xia"
#define  VERSION	"0.0.1"

#define  MSQ_NAME "."
#define  MAX_NAME  20
#define  BUF_SIZE	255
/*
 *protocol:
  ---------------------------------------------------
 |CMD(1B):DST_ID(2B):TIME(4B):key(2B):MSG_SIZE(2B):MSG|
  ---------------------------------------------------
 MAX_SIZE: 255
 * */
#define  KEY		0x1234
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
	char mtext[BUF_SIZE];
};

struct  user_info {
	int id;
	char name[MAX_NAME];
	struct list_head list;
};


