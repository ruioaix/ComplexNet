MAKEROOT := $(shell pwd)
INCLUDE_DIR := $(MAKEROOT)/src
CC := gcc
CFLAG :=  -g -Wall -Wunused 
common_objs = 	obj/error.o \
				obj/bip.o \
				obj/iidlinefile.o \
				obj/iidnet.o \
				obj/mt_random.o \
				obj/sort.o \
				obj/iilinefile.o \
				obj/hashtable.o


#common_inc = $(common_objs:.o=.d)


.PHONY : all clean

all: onion 


onion : $(common_objs) obj/main/onion.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

reappearLLY: $(common_objs) obj/main/reappearLLY.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

selectRandomUsers: $(common_objs) obj/main/selectRandomUsers.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAG) -c $< -o $@

obj/main/%.o: src/main/%.c
	$(CC) $(CFLAG) -I$(INCLUDE_DIR) -c $< -o $@

#include $(common_inc)
#
#obj/%.d: src/%.c
#	set -e; rm -f $@; \
#	$(CC) -MM  $< > $@.$$$$; \
#	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
#	rm -f $@.$$$$


main_objs = 	obj/main/onion.o\
				obj/main/reappearLLY.o\
				obj/main/selectRandomUsers.o

clean : 
	$(RM) $(main_objs)
	$(RM) $(common_objs)
	$(RM) onion reappearLLY selectRandomUsers
