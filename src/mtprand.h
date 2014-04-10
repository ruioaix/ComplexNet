#ifndef CN_MTPRAND_H
#define CN_MTPRAND_H

void set_seed_MTPR(unsigned long seed);
void set_timeseed_MTPR(void);

unsigned long get_i32_MTPR(void);
long get_i31_MTPR(void);
double get_d01_MTPR(void);
double get_d0_MTPR(void);
double get_d_MTPR(void);
double get_d530_MTPR(void);

#endif
