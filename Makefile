CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

tycc: $(OBJS)
	$(CC) -o tycc $(OBJS) $(LDFLAGS)

$(OBJS): tycc.h

test: tycc
	./test.sh

clean:
	rm -f tycc *.o *~ tmp*

.PHONY: test clean
