MAKEROOT := $(shell pwd)
INCLUDE_DIR := $(MAKEROOT)/src
CC := gcc


common_objs = 	obj/error.o \
				obj/bip.o \
				obj/iidlinefile.o \
				obj/iidnet.o \
				obj/mt_random.o \
				obj/sort.o \
				obj/iilinefile.o

main_objs = 	obj/main/onion.o

.PHONY : all clean

all: onion

onion : $(common_objs) obj/main/onion.o
	$(CC) -lm $^ -o $@ 

obj/%.o: src/%.c 
	$(CC)  -c $< -o $@

obj/main/%.o: main/%.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@

clean : 
	$(RM) $(common_objs)
	$(RM) $(main_objs)
	$(RM) onion
