run : main.c inc/*.h src/*.c
	gcc -Wall -Wunused -g main.c src/*.c src/linefile/*.c src/utility/*.c -o run -lm -lpthread 

linefile.o : src/linefile/*linefile.c
	gcc -c src/linefile/*linefile.c
	ar rcs libraries/liblinefile.a *.o
	rm *.o
