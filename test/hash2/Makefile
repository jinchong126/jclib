STCC=gcc -Wall -g -c -o
STLK=gcc -o
AR=ar -rv

INC_DIR+= -I/root/test/WXEngine_0419/trunk/src/libsrc/include -I./include

LIB_BASE+= -L/root/test/WXEngine_0419/trunk/lib -lbase -lpthread

SOURCE_FILE_LIST += test.c

OBJ_BASE_DIR:=$(patsubst %.c, %.o, $(SOURCE_FILE_LIST))

OBJ_BASE:=$(notdir $(OBJ_BASE_DIR))

.PHONY=all clean

all:test clean

test:$(OBJ_BASE_DIR)
	@$(STLK) $@ $(OBJ_BASE_DIR) $(LIB_BASE)
	@echo "    LK  $@"

clean:
	@rm -f *.o

%.o: %.c
	@$(STCC) $@ $(INC_DIR) $<
	@echo "    CC " $<
