CC = gcc

MACRO = -Wall

# nothing: 
#         @echo missing make tag

all: queue

queue: queue.c threadpool.c
	$(CC) -o $@ queue.c threadpool.c -lpthread

clean:
	rm -f *.o queue
