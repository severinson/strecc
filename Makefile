#
# Sample Makefile for lsh
# lab1 in Operating System course
#
BIN=	strecc

SRCS=	$(wildcard *.c)
OBJS=	$(patsubst %.c,%.o,$(SRCS))

CC=	gcc
CFLAGS= -g -Wall -pthread

LIBS= -lreadline -ltermcap

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

all:	$(BIN)

strecc:	$(OBJS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS) $(LIBS)

clean:
	-rm -f $(OBJS) strecc
