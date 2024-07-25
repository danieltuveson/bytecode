CC=gcc
CFLAGS = -O2 -g -Wall -Wextra

bytecode: bytecode.o
	$(CC) $(CFLAGS) -DLOOPED bytecode.c -o looped
	$(CC) $(CFLAGS) -DTHREADED bytecode.c -o threaded

clean:
	rm -f bytecode.o looped threaded
