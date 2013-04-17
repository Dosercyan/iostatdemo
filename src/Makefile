CC=gcc
CFLAG=-Wall -g
CIFLAG=-I.
CLFLAG=

all: iostat.o

iostat.o: iostat.c
	$(CC) $(CFLAG) $(CIFLAG) $(CLFLAG) -o $@ -c $<

clean:
	rm -rf iostat.o