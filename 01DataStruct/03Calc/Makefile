TGT := test
SRCS :=  test.c calc.c stack.c dlist.c
CC ?= gcc
CFLAGS +=  -g -DDEBUG


all :  $(TGT)

$(TGT) : ${SRCS:%.c=%.o}
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean all

clean: 
	$(RM) *.o $(TGT)
