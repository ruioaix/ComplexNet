run : main.c inc/*.h src/*.c
	gcc -Wall -Wunused -g -lm main.c src/*.c -o run 
