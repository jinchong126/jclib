STCC=gcc -g -c
STLK=gcc -o
AR=ar -rv

INC_DIR = -I./include

LIB_BASE = -L./ -lpthread -lm -lc

OBJ_TEST=rbtest1.o rbtree.o rbtree_com.o

all: rbtest clean

rbtest:$(OBJ_TEST)
	$(STLK) $@ $(OBJ_TEST) $(LIB_BASE)

clean:
	rm -f *.o core

rbtest1.o:rbtest1.c
	$(STCC) $(INC_DIR) rbtest1.c
rbtree.o:rbtree.c
	$(STCC) $(INC_DIR) rbtree.c
rbtree_com.o:rbtree_com.c
	$(STCC) $(INC_DIR) rbtree_com.c
