MAKEROOT := $(shell pwd)
INCLUDE_DIR := $(MAKEROOT)/src
CC := gcc
CFLAG :=  -Wall -Wunused 
common_objs = 	obj/error.o \
				obj/bip.o \
				obj/iidlinefile.o \
				obj/iidnet.o \
				obj/mt_random.o \
				obj/sort.o \
				obj/iilinefile.o


common_inc = $(common_objs:.o=.d)


.PHONY : all clean

all: onion reappearLLY


include $(common_inc)


obj/%.d: src/%.c
	set -e; rm -f $@; \
	$(CC) -MM  $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

onion : $(common_objs) obj/main/onion.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

reappearLLY: $(common_objs) obj/main/reappearLLY.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

obj/%.o: src/%.c 
	$(CC) $(CFLAG) -c $< -o $@

obj/main/%.o: src/main/%.c
	$(CC) $(CFLAG) -I$(INCLUDE_DIR) -c $< -o $@

main_objs = 	obj/main/onion.o\
				obj/main/reappearLLY.o


clean : 
	$(RM) $(main_objs)
	$(RM) $(common_objs)
	$(RM) $(common_inc)
	$(RM) onion reappearLLY
