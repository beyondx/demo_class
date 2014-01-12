TGT := server client
SRCS := server.c client.c
CFLAGS += -D_DEBUG -g

all: $(TGT)

% : %.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	$(RM) $(TGT) *.o
