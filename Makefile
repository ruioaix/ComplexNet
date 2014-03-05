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

common_inc = $(common_objs:.o=.d)

all: onion

#include $(common_objs:.o=.d)
include $(common_inc)

obj/%.d: src/%.c
	set -e; rm -f $@; \
	$(CC) -MM  $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY : all clean

onion : $(common_objs) obj/main/onion.o
	$(CC) -lm $^ -o $@ 

obj/%.o: src/%.c 
	$(CC)  -c $< -o $@

obj/main/%.o: src/main/%.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@

clean : 
	$(RM) $(common_objs)
	$(RM) $(main_objs)
	$(RM) $(common_inc)
	$(RM) onion
