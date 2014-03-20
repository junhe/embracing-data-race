# Define required macros here
SHELL=/bin/sh

OBJS=main.o
CFLAGS=-Wall -lpthread -lrt -D_FILE_OFFSET_BITS=64
CC=g++
INCLUDES=
LIBS=

all: counter-drf
counter-dfr: counter-drf.o
	${CC} ${CFLAGS} ${INCLUDES} -o $@ counter-drf.o ${LIBS}

clean:
	-rm -f *.o core *.core *.gch

.cpp.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $<


