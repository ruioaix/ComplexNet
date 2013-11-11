#ifndef BITSIGN_H
#define BITSIGN_H

#include <limits.h>

enum {BITS_PER_WORD=sizeof(unsigned int)*CHAR_BIT};

#define WORD_OFFSET(b) ((b)/BITS_PER_WORD)
#define BIT_OFFSET(b) ((b)%BITS_PER_WORD)

unsigned int *createBitSign(int signMax);
int getBitSign(unsigned int *signStick, int signId);
void setBitSign(unsigned int *signStick, int signId);
void clearBitSign(unsigned int *signStick, int signId);

#endif
