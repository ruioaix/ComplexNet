#ifndef THREADSAFE_RANDOM_H
#define THREADSAFE_RANDOM_H

void init_threadsafe(void);
void init_genrand_threadsafe(unsigned long s, int t);
int init_by_array_threadsafe(unsigned long init_key[], int key_length);
unsigned long genrand_int32_threadsafe(int t);
double genrand_real1_threadsafe(int t);
double genrand_real2_threadsafe(int t);
void PrintCurrent_ID(void);
void free_threadsafe(int t);

#endif
