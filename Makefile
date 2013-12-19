run : main.c inc/*/*.h  src/*/*.c
	gcc -Wall -Wunused -g main.c src/*/*.c -o run -lm -lpthread 

run1 : main1.c inc/*/*.h  src/*/*.c
	gcc -Wall -Wunused -g main1.c src/*/*.c -o run1 -lm -lpthread 

thread : main.c inc/*/*.h src/*/*.c
	gcc -Wall -Wunused -g main.c src/*/*.c -o run -lm -lpthread 

linefile.o : src/linefile/*linefile.c
	gcc -c src/linefile/*linefile.c
	ar rcs libraries/liblinefile.a *.o
	rm *.o
