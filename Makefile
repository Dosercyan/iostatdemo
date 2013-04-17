CC=gcc
CFLAG=-Wall -g
CIFLAG=-I.
CLFLAG=

OBJS=./src/iostat.o
SUBDIRS=src
MAKE=make

all: iostatdemo

iostatdemo: 
	for subdir in $(SUBDIRS); \
        do \
        echo "making in $$subdir"; \
        ( cd $$subdir && $(MAKE) all -f Makefile) || exit 1; \
        done
		$(CC) $(CLFLAGS) $(CIFLAG) $(CFLAG) -o $@ $(OBJS)


clean:
	@for dir in $(SUBDIRS); do make -C $$dir clean|| exit 1; done
	rm -rf iostatdemo
