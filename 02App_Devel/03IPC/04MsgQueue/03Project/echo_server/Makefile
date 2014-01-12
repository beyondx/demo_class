TGT := server client
CFLAGS := -Wall
DEBUG := y

ifeq  ($(DEBUG), y)
	CFLAGS += -g -DDEBUG
endif


all: $(TGT)

% : %.c
	$(CC) $(CFLAGS)  $< -o $@

.PHONY: clean all
clean:
	$(RM) $(TGT)
