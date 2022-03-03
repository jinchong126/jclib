STCC=gcc -g -c
STLK=gcc -o
AR=ar -rv

INC_DIR = -I./include

LIB_BASE = -L./ -lpthread 

OBJ_TEST=timerlib.o test.o

all: test clean


test:$(OBJ_TEST)
	$(STLK) $@ $(OBJ_TEST) $(LIB_BASE)

clean:
	rm -f *.o core

timerlib.o:timerlib.c
	$(STCC) $(INC_DIR) timerlib.c

test.o:test.c
	$(STCC) $(INC_DIR) test.c
