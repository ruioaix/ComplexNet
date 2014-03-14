MAKEROOT := $(shell pwd)
INCLUDE_DIR := -I$(MAKEROOT)/src # -I$(MAKEROOT)/src/main
CC := gcc
CFLAG :=  -g -Wall -Wunused 
common_objs = 	obj/error.o \
				obj/bip.o \
				obj/iidlinefile.o \
				obj/iidnet.o \
				obj/mt_random.o \
				obj/sort.o \
				obj/iilinefile.o \
				obj/hashtable.o\
				obj/recommend.o



.PHONY : all clean


#################################################################
onion_topS : $(common_objs) obj/main/onion_topS.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

similarity : $(common_objs) obj/main/similarity.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

onion : $(common_objs) obj/main/onion.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

bip : $(common_objs) obj/main/bip.o 
	$(CC) $(CFLAG) -lm $^ -o $@ 

all: bip onion reappearLLY selectRandomUsers

reappearLLY: $(common_objs) obj/main/reappearLLY.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

selectRandomUsers: $(common_objs) obj/main/selectRandomUsers.o
	$(CC) $(CFLAG) -lm $^ -o $@ 
#################################################################



#################################################################
obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAG) -c $< -o $@

obj/main/%.o: main/%.c
	$(CC) $(CFLAG) $(INCLUDE_DIR) -c $< -o $@
#################################################################



#################################################################
main_objs = 	obj/main/onion.o\
				obj/main/reappearLLY.o\
				obj/main/selectRandomUsers.o\
				obj/main/bip.o
main_exec = onion\
			reappearLLY\
			selectRandomUsers\
			bip
clean : 
	$(RM) $(main_objs)
	$(RM) $(common_objs)
	$(RM) $(main_exec)
#################################################################
