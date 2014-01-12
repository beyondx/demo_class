CFLAGS += -g -DDEBUG -Wall
SRCS := test.c hashtable.c
TGT  := test


all : $(TGT) 

$(TGT) : ${SRCS:%.c=%.o}
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean all

clean:
	$(RM) ${SRCS:%.c=%.o} ${TGT}
