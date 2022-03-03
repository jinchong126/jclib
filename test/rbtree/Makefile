CC = gcc

HEAD = rbtree.h rbtree_rc.h
SRC = rbtree.c
DST = rbtest

MACRO = -Wall

# nothing: 
#         @echo missing make tag

all: rbtest1 rbtest2

rbtest1: $(HEAD) $(SRC) rbtree_rc.c rbtest1.c
	$(CC) -o $@ $(SRC) rbtree_rc.c rbtest1.c

rbtest2: $(HEAD) $(SRC) rbtree_rc.c rbtest2.c
	$(CC) -o $@ $(SRC) rbtree_rc.c rbtest2.c

clean:
	rm -rf rbtest1 rbtest2
