STCC=gcc -Wall -c
STLK=gcc -o

OBJ=cache.o

LIB=-lpthread

all:cache clean
cache:$(OBJ)
	$(STLK) $@ $(OBJ) $(LIB)
clean:
	rm -f *.o

cache.o:cache.c
	$(STCC) cache.c
