TGT := a.out
SRCS := address_list.c main.c save.c
CFLAGS := -g -Wall -DNDEBUG_WEIGHT
DEP := dep
CC ?= gcc

all: $(TGT)

$(TGT) : ${SRCS:%.c=%.o}
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $<

sinclude $(DEP)

$(DEP): $(SRCS)
	$(CC) $(CFLAGS) -MM $^ > $@

.PHONY : clean all $(DEP)

clean :
	$(RM) $(TGT) *.o $(DEP)
