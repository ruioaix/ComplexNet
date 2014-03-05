objs = 	obj/onion.o\
		obj/error.o\
		obj/bip.o\
		obj/iidlinefile.o\
		obj/iidnet.o\
		obj/mt_random.o\
		obj/sort.o\
		obj/iilinefile.o

CC=gcc
.PHONY : all clean

all: onion

onion : $(objs)
	$(CC) -o $@ -lm $(objs)

#$(objs) : %.o : %.c
#	cc -c $< -o $@
obj/%.o: src/%.c
	$(CC)  -c $< -o $@

#onion.o : 
#iidnet.o : 
#iilinefile.o : iilinefile.h
#iidlinefile.o : iidlinefile.h
#bip.o : bip.h
#error.o : error.h
#mt_random.o : mt_random.c
#sort.o : sort.h

clean : 
	$(RM) onion $(objs)
