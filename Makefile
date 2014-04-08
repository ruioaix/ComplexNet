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

OBJ_DIR = obj $(patsubst %,obj/%, $(MAIN_ALL_DIR) $(SRC_DIR))

#include dir
INCLUDE_DIR := $(patsubst %, -I%, $(SRC_DIR) $(MAIN_ALL_DIR))

CC := gcc
CFLAG :=  -g -Wall -Wunused 


.PHONY : dir all clean

all: dir tdpotn-ndhctheta
#all: dir $(MAIN_ALL_EXEC)

dir: $(OBJ_DIR)

#################################################################
MAIN_TDPOTN_SRC = $(wildcard main/tdpotn/*.c)
MAIN_TDPOTN_EXEC = $(subst /,-, $(patsubst main/%.c, %, $(MAIN_TDPOTN_SRC)))
$(MAIN_TDPOTN_EXEC) : tdpotn-% : $(SRC_OBJ) obj/main/tdpotn/%.o 
	$(CC) $(CFLAG) -lm $^ -o $@ 

MAIN_ONION_SRC = $(wildcard main/onion/*.c)
MAIN_ONION_EXEC = $(subst /,-, $(patsubst main/%.c, %, $(MAIN_ONION_SRC)))
$(MAIN_ONION_EXEC) : onion-% : $(SRC_OBJ) obj/main/onion/%.o 
	$(CC) $(CFLAG) -lm $^ -o $@ 

MAIN_GENERATENET_SRC = $(wildcard main/generatenet/*.c)
MAIN_GENERATENET_EXEC = $(subst /,-, $(patsubst main/%.c, %, $(MAIN_GENERATENET_SRC)))
$(MAIN_GENERATENET_EXEC) : generatenet-% : $(SRC_OBJ) obj/main/generatenet/%.o 
	$(CC) $(CFLAG) -lm $^ -o $@ 

MAIN_SRC = $(wildcard main/*.c) 
MAIN_EXEC = $(basename $(notdir $(MAIN_SRC)))
$(MAIN_EXEC) : % : $(SRC_OBJ) obj/main/%.o
	$(CC) $(CFLAG) -lm $^ -o $@ 
#################################################################



#################################################################
obj/src/%.o : src/%.c src/%.h 
	$(CC) $(CFLAG) $(INCLUDE_DIR) -c $< -o $@

obj/main/%.o: main/%.c 
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
#################################################################
