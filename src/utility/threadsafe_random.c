
//modified by rui. 2013.08.09
#include "../../inc/utility/threadsafe_random.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */
#define Thread_Safe_MAX 100

//static unsigned long mt[N]; /* the array for the state vector  */
//static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

static unsigned long mt_a[Thread_Safe_MAX][N]; /* the array for the state vector  */
static int mti_a[Thread_Safe_MAX]; /* mti==N+1 means mt[N] is not initialized */
static int MersenneTwister_Valid_ID[Thread_Safe_MAX];
static int MersenneTwister_Using_ID[Thread_Safe_MAX];
static int MersenneTwister_Valid_IdNum;
static int MersenneTwister_Using_IdNum;

static pthread_mutex_t mutex;


void init_threadsafe(void) {
	pthread_mutex_init(&mutex, NULL);
	MersenneTwister_Valid_IdNum=Thread_Safe_MAX;
	int i;
	for (i=0;i<Thread_Safe_MAX; ++i) {
		MersenneTwister_Valid_ID[i]=i;
	}
	MersenneTwister_Using_IdNum=0;
}

//should not be used, if you don't want to different random number.
/* initializes mt[N] with a seed */
void init_genrand_threadsafe(unsigned long s, int t)
{
    mt_a[t][0]= s & 0xffffffffUL;
    for (mti_a[t]=1; mti_a[t]<N; mti_a[t]++) {
        mt_a[t][mti_a[t]] = 
	    (1812433253UL * (mt_a[t][mti_a[t]-1] ^ (mt_a[t][mti_a[t]-1] >> 30)) + mti_a[t]); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt_a[t][].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt_a[t][mti_a[t]] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
int init_by_array_threadsafe(unsigned long init_key[], int key_length)
{
	
	pthread_mutex_lock(&mutex);
	--MersenneTwister_Valid_IdNum;
	if (MersenneTwister_Valid_ID<0) {
		printf("too much threads for MT PRNG\n");
		exit(-1);
	}
	int t = MersenneTwister_Valid_ID[MersenneTwister_Valid_IdNum];
	MersenneTwister_Using_ID[MersenneTwister_Using_IdNum]=t;
	++MersenneTwister_Using_IdNum;
	pthread_mutex_unlock(&mutex);

    int i, j, k;
    init_genrand_threadsafe(19650218UL, t);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--) {
        mt_a[t][i] = (mt_a[t][i] ^ ((mt_a[t][i-1] ^ (mt_a[t][i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt_a[t][i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt_a[t][0] = mt_a[t][N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt_a[t][i] = (mt_a[t][i] ^ ((mt_a[t][i-1] ^ (mt_a[t][i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt_a[t][i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt_a[t][0] = mt_a[t][N-1]; i=1; }
    }

    mt_a[t][0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 

	//printf("%lu\n", mti);
	//for(i=0; i<N; ++i) {
	//	printf("%luul, ", mt_a[t][i]);
	//}
	//printf("\n");
	return t;
}

void free_threadsafe(int t) {
	pthread_mutex_lock(&mutex);
	MersenneTwister_Valid_ID[MersenneTwister_Valid_IdNum] = t;
	++MersenneTwister_Valid_IdNum;
	if (MersenneTwister_Valid_IdNum>Thread_Safe_MAX) {
		printf("it looks like you free too much MT PRNG.\n");
		exit(-1);
	}
	int i;
	for (i=0; i<MersenneTwister_Using_IdNum; ++i) {
		if (MersenneTwister_Using_ID[i] == t) {
			MersenneTwister_Using_ID[i] = MersenneTwister_Using_ID[--MersenneTwister_Using_IdNum];
		}
	}
	pthread_mutex_unlock(&mutex);
}

void PrintCurrent_ID(void) {
	pthread_mutex_lock(&mutex);
	int i;
	for (i=0; i<MersenneTwister_Valid_IdNum; ++i) {
		printf("%d\t", MersenneTwister_Valid_ID[i]);
	}
	printf("\n%d\n", MersenneTwister_Valid_IdNum);
	for (i=0; i<MersenneTwister_Using_IdNum; ++i) {
		printf("%d\t", MersenneTwister_Using_ID[i]);
	}
	printf("\n%d\n", MersenneTwister_Using_IdNum);
	pthread_mutex_unlock(&mutex);
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32_threadsafe(int t)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti_a[t] >= N) { /* generate N words at one time */
        int kk;

        if (mti_a[t] == N+1)   /* if init_genrand() has not been called, */
            init_genrand_threadsafe(5489UL, t); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt_a[t][kk]&UPPER_MASK)|(mt_a[t][kk+1]&LOWER_MASK);
            mt_a[t][kk] = mt_a[t][kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt_a[t][kk]&UPPER_MASK)|(mt_a[t][kk+1]&LOWER_MASK);
            mt_a[t][kk] = mt_a[t][kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt_a[t][N-1]&UPPER_MASK)|(mt_a[t][0]&LOWER_MASK);
        mt_a[t][N-1] = mt_a[t][M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti_a[t] = 0;
    }
  
    y = mt_a[t][mti_a[t]++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1]-real-interval */
double genrand_real1_threadsafe(int t)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti_a[t] >= N) { /* generate N words at one time */
        int kk;

        if (mti_a[t] == N+1)   /* if init_genrand() has not been called, */
            init_genrand_threadsafe(5489UL, t); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt_a[t][kk]&UPPER_MASK)|(mt_a[t][kk+1]&LOWER_MASK);
            mt_a[t][kk] = mt_a[t][kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt_a[t][kk]&UPPER_MASK)|(mt_a[t][kk+1]&LOWER_MASK);
            mt_a[t][kk] = mt_a[t][kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt_a[t][N-1]&UPPER_MASK)|(mt_a[t][0]&LOWER_MASK);
        mt_a[t][N-1] = mt_a[t][M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti_a[t] = 0;
    }
  
    y = mt_a[t][mti_a[t]++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1]-real-interval */
double genrand_real2_threadsafe(int t)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti_a[t] >= N) { /* generate N words at one time */
        int kk;

        if (mti_a[t] == N+1)   /* if init_genrand() has not been called, */
            init_genrand_threadsafe(5489UL, t); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt_a[t][kk]&UPPER_MASK)|(mt_a[t][kk+1]&LOWER_MASK);
            mt_a[t][kk] = mt_a[t][kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt_a[t][kk]&UPPER_MASK)|(mt_a[t][kk+1]&LOWER_MASK);
            mt_a[t][kk] = mt_a[t][kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt_a[t][N-1]&UPPER_MASK)|(mt_a[t][0]&LOWER_MASK);
        mt_a[t][N-1] = mt_a[t][M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti_a[t] = 0;
    }
  
    y = mt_a[t][mti_a[t]++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y*(1.0/4294967296.0); 
    /* divided by 2^32-1 */ 
}
