# Define required macros here
SHELL=/bin/sh

OBJS=main.o
CFLAGS=-Wall -lpthread -lrt -D_FILE_OFFSET_BITS=64
CC=g++
INCLUDES=
LIBS=

all: hashtable

counter: counter.o Util.o
	${CC} ${CFLAGS} ${INCLUDES} -o $@ Util.o counter.o ${LIBS}

hashtable: hashtable.o Util.o
	${CC} ${CFLAGS} ${INCLUDES} -o $@ Util.o hashtable.o ${LIBS}

clean:
	-rm -f *.o core *.core *.gch

.cpp.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $<


