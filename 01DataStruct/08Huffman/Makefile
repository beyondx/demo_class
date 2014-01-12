TGT := test
SRCS := test.c huff.c sort_list.c
#CFLAGS += -Wall -DDEBUG -g 
CFLAGS += -Wall -D_DEBUG -g 
all: $(TGT)

$(TGT) : $(SRCS:%.c=%.o)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean all

clean:
	$(RM) $(SRCS:%.c=%.o) $(TGT)
