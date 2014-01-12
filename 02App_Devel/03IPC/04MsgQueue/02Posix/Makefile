TGT := mq_rcv mq_snd
CFLAGS := -Wall -lrt
DEBUG := y

ifeq  ($(DEBUG), y)
	CFLAGS += -g
endif


all: $(TGT)

% : %.c
	$(CC) $(CFLAGS)  $< -o $@

.PHONY: clean all
clean:
	$(RM) $(TGT)
