
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
MAIN_SRC = $(wildcard main/*.c) $(wildcard main/*/*.c)
MAIN_DIR = $(sort $(dir $(MAIN_SRC)))
MAIN_OBJ = $(patsubst %.c, obj/%.o, $(MAIN_SRC))

#include dir
INCLUDE_DIR := $(patsubst %, -I%, $(SRC_DIR) $(MAIN_DIR))

CC := gcc
CFLAG :=  -g -Wall -Wunused 

#get all exec name, if onion/bip.c, then exec name is onion-bip.
#if similarity.c, then exec name is similarity.
MAIN_EXEC = $(subst /,-, $(patsubst main/%.c, %, $(MAIN_SRC)))

.PHONY : all clean

all: 
	@echo $(SRC_DIR)
	@echo $(MAIN_DIR)
	@echo $(src_obj)
	@echo $(INCLUDE_DIR)


#################################################################
onion_%: $(common_objs) obj/main/onion_%.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

$(main_exec) : $(src_obj) obj/main/%.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

#################################################################



#################################################################
$(SRC_DIR)%.o : 
obj/src/%.o: src/%.c src/%.h
	$(CC) $(CFLAG) $(INCLUDE_DIR) -c $< -o $@

obj/main/%.o: main/%.c
	$(CC) $(CFLAG) $(INCLUDE_DIR) -c $< -o $@
#################################################################



#################################################################
main_objs = 	obj/main/onion.o\
				obj/main/reappearLLY.o\
				obj/main/selectRandomUsers.o\
				obj/main/similarity.o\
				obj/main/bip.o\
				$(wildcard obj/main/onion_*.o)


clean : 
	$(RM) $(main_objs)
	$(RM) $(src_obj)
	$(RM) $(main_exec)
#################################################################
