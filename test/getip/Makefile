STCC=gcc -g -c
STLK=gcc -o
AR=ar -rv

LIB_DIR=/root/test/jclib

INC_DIR = -I$(LIB_DIR)/include

LIB_BASE = -L$(LIB_DIR) -lbase -lpthread -lm -lc

OBJ_TEST=test.o

all: test clean

test:$(OBJ_TEST)
	$(STLK) $@ $(OBJ_TEST) $(LIB_BASE)

clean:
	rm -f *.o core

test.o:test.c
	$(STCC) $(INC_DIR) test.c
