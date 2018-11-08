.PHONY: all clean

CFLAGS+=
LDFLAGS+=-lm     # link to math library

all: train.c test.c
	gcc train.c -o train
	gcc test.c -o test

clean:
	rm -f train test