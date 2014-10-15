cflags=-g -Wall

all: strecc

strecc: ccsv.o logging.o db.o administration.o

clean:
	rm -f *.o
	rm -f strecc
