STCC=gcc -MMD -Wall -Werror -g -c -o
STLK=gcc -o
AR=ar -rv

INC_DIR+= -I./include

#LIB_BASE+= -L./lib

SOURCE_FILE_LIST += dictionary.c
SOURCE_FILE_LIST += iniparser.c
SOURCE_FILE_LIST += system.c
SOURCE_FILE_LIST += string.c
SOURCE_FILE_LIST += rbtree.c
SOURCE_FILE_LIST += rbtree_com.c
SOURCE_FILE_LIST += hash_list.c
SOURCE_FILE_LIST += log.c
SOURCE_FILE_LIST += time.c
SOURCE_FILE_LIST += net.c
SOURCE_FILE_LIST += cache.c
SOURCE_FILE_LIST += lfqueue.c
SOURCE_FILE_LIST += timerqueue.c

OBJ_BASE_DIR:=$(patsubst %.c, %.o, $(SOURCE_FILE_LIST))

OBJ_BASE:=$(notdir $(OBJ_BASE_DIR))

.PHONY=all clean

all:libbase.a

libbase.a:$(OBJ_BASE_DIR)
	@$(AR) $@ $(OBJ_BASE_DIR) $(LIB_BASE) >/dev/NULL
	@echo "    AR " $@
	mv $@ lib

clean:
	@rm -f *.o *.d

%.o: %.c
	@$(STCC) $@ $(INC_DIR) $<
	@echo "    CC " $<

-include $(OBJ_BASE:%.o=%.d)
