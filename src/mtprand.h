/**
 * this file is used to generate Pseudo-random number, origin from Mersenne Twister.
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c
 * user can use get_xxx_MTPR without calling set_xxseed_MTPR first,
 * 	because I already set a default seed.
 * but if you want to get a different pseudo-random number list,
 * 	you can set a different seed explicitly, or use current unix timestamp as seed.
 * 
 * Author: RuiXiao <xrfind@gmail.com>
 */

#ifndef CN_MTPRAND_H
#define CN_MTPRAND_H

/// set a explicit seed.
extern void set_seed_MTPR(unsigned long seed);
/// set the seed with unix timestamp.
extern void set_timeseed_MTPR(void);

/// get a 32 bits prn.
extern unsigned long get_i32_MTPR(void);

/// get a 31 bits prn.
extern long get_i31_MTPR(void);

/// get a double prm which is in [0, 1].
extern double get_d01_MTPR(void);

/// get a double prm which is in [0, 1).
extern double get_d0_MTPR(void);

/// get a double prm which is in (0, 1).
extern double get_d_MTPR(void);

/// get a 53-precision double prm which is in [0, 1).
extern double get_d530_MTPR(void);

#endif
