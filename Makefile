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
onion_3parts_topR: $(common_objs) obj/main/onion_3parts_topR.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

onion_3parts_bestR: $(common_objs) obj/main/onion_3parts_bestR.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

onion_expR : $(common_objs) obj/main/onion_expR.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

onion_corS : $(common_objs) obj/main/onion_corS.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

onion_corR : $(common_objs) obj/main/onion_corR.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

onion_topR : $(common_objs) obj/main/onion_topR.o
	$(CC) $(CFLAG) -lm $^ -o $@ 

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
				obj/main/similarity.o\
				obj/main/bip.o\
				obj/main/onion_topR.o\
				obj/main/onion_corR.o\
				obj/main/onion_topS.o\
				obj/main/onion_corS.o\
				obj/main/onion_expR.o\
				obj/main/onion_3parts_bestR.o\
				obj/main/onion_3parts_topR.o

main_exec = onion\
			reappearLLY\
			selectRandomUsers\
			similarity\
			bip\
			onion_topR\
			onion_corR\
			onion_topS\
			onion_corS\
			onion_expR\
			onion_3parts_bestR\
			onion_3parts_topR
clean : 
	$(RM) $(main_objs)
	$(RM) $(common_objs)
	$(RM) $(main_exec)
#################################################################
