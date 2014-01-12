TGT := test
SRCS :=  test.c  btree.c queue.c stack.c dlist.c
CC ?= gcc

all :  $(TGT)

$(TGT) : ${SRCS:%.c=%.o}
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $<

.PHONY: clean all

clean: 
	$(RM) *.o $(TGT)
