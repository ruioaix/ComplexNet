#prerequisite: no src/*/*/*.c
#get all source file name in src, include pathname.
#get all obj file name in obj.
#get all directory name in src.
SRC_SRC = $(wildcard src/*.c) $(wildcard src/*/*c)
SRC_DIR = $(sort $(dir $(SRC_SRC)))
SRC_OBJ = $(patsubst %.c, obj/%.o, $(SRC_SRC))

#prerequisite: no main/*/*/*.c
#get all source file name in main, include pathname.
#get all obj file name in obj.
#get all directory name in main.
MAIN_ALL_SRC = $(wildcard main/*.c) $(wildcard main/*/*.c)
MAIN_ALL_DIR = $(sort $(dir $(MAIN_ALL_SRC)))
MAIN_ALL_OBJ = $(patsubst %.c, obj/%.o, $(MAIN_ALL_SRC))
MAIN_ALL_EXEC = $(subst /,-, $(patsubst main/%.c, %, $(MAIN_ALL_SRC)))

TEST_SRC = $(wildcard test/*.c test/*/*.c)
TEST_DIR = $(sort $(dir $(TEST_SRC)))
TEST_OBJ = $(patsubst %.c, obj/%.o, $(TEST_SRC))

OBJ_DIR = obj $(patsubst %,obj/%, $(MAIN_ALL_DIR) $(SRC_DIR) $(TEST_DIR))

#include dir
INCLUDE_DIR := $(patsubst %, -I%, $(SRC_DIR) $(MAIN_ALL_DIR))

CC := gcc
CFLAG :=  -std=c99 -g -Wall -Wunused 


.PHONY : dir all clean test tar tdpotn

#all : dir test
#all: dir robust-base
all: dir robust-linkcp
#all: dir tdpotn-wair
#all: dir tdpotn-coupling
#all: dir tdpotn-gini 
#all: dir test-linefile
#all: dir score-3methods
#all: dir test-dividebip
#all: dir $(MAIN_ALL_EXEC)

dir: $(OBJ_DIR)

tar: main src Makefile
	@tar zcf complexnet.tar.gz $^


#################################################################
MAIN_SCORE_SRC = $(wildcard main/score/*.c)
MAIN_SCORE_EXEC = $(subst /,-, $(patsubst main/%.c, %, $(MAIN_SCORE_SRC)))
$(MAIN_SCORE_EXEC) : score-% : $(SRC_OBJ) obj/main/score/%.o 
	$(CC) $(CFLAG) -lm $^ -o $@ 

MAIN_TDPOTN_SRC = $(wildcard main/tdpotn/*.c)
MAIN_TDPOTN_EXEC = $(subst /,-, $(patsubst main/%.c, %, $(MAIN_TDPOTN_SRC)))
$(MAIN_TDPOTN_EXEC) : tdpotn-% : $(SRC_OBJ) obj/main/tdpotn/%.o 
	$(CC) $(CFLAG) -lm $^ -o $@ 

MAIN_ROBUST_SRC = $(wildcard main/robust/*.c)
MAIN_ROBUST_EXEC = $(subst /,-, $(patsubst main/%.c, %, $(MAIN_ROBUST_SRC)))
$(MAIN_ROBUST_EXEC) : robust-% : $(SRC_OBJ) obj/main/robust/%.o 
	$(CC) $(CFLAG) -lm $^ -o $@ 

MAIN_ONION_SRC = $(wildcard main/onion/*.c)
MAIN_ONION_EXEC = $(subst /,-, $(patsubst main/%.c, %, $(MAIN_ONION_SRC)))
$(MAIN_ONION_EXEC) : onion-% : $(SRC_OBJ) obj/main/onion/%.o 
	$(CC) $(CFLAG) -lm $^ -o $@ 

MAIN_TOOL_SRC = $(wildcard main/tool/*.c)
MAIN_TOOL_EXEC = $(subst /,-, $(patsubst main/%.c, %, $(MAIN_TOOL_SRC)))
$(MAIN_TOOL_EXEC) : tool-% : $(SRC_OBJ) obj/main/tool/%.o 
	$(CC) $(CFLAG) -lm $^ -o $@ 

MAIN_SRC = $(wildcard main/*.c) 
MAIN_EXEC = $(basename $(notdir $(MAIN_SRC)))
$(MAIN_EXEC) : % : $(SRC_OBJ) obj/main/%.o
	$(CC) $(CFLAG) -lm $^ -o $@ 
#################################################################

tdpotn : $(MAIN_TDPOTN_EXEC)

#################################################################
test : test/run
#	-@./test/main

test/run : $(SRC_OBJ) $(TEST_OBJ)
	$(CC) -lm $^ -o $@
#################################################################


#################################################################
obj/src/%.o : src/%.c  src/%.c src/base.h
	$(CC) $(CFLAG) $(INCLUDE_DIR) -c $< -o $@

obj/main/%.o: main/%.c 
	$(CC) $(CFLAG) $(INCLUDE_DIR) -c $< -o $@

obj/test/%.o : test/%.c test/test.h 
	$(CC) $(CFLAG) $(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR) :
	mkdir -p $@
#################################################################



#################################################################
clean : 
	$(RM) $(MAIN_ALL_OBJ)
	$(RM) $(SRC_OBJ)
	$(RM) $(MAIN_ALL_EXEC)
	$(RM) -r $(OBJ_DIR)
	$(RM) tags test/run complexnet.tar.gz
#################################################################
