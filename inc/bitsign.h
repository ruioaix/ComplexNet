#ifndef BITSIGN_H
#define BITSIGN_H

#include "errorhandle.h"
#include "stdint.h" //for uint32_t
#include "limits.h" //for CHAR_BIT

enum {BITS_PER_WORD=sizeof(uint32_t)*CHAR_BIT};

#define WORD_OFFSET(b) ((b)/BITS_PER_WORD)
#define BIT_OFFSET(b) ((b)%BITS_PER_WORD)

//use one bit to sign a vt's status. 
//becuse of using bit, the memory usage is small. 
//becuse of using bit, vt can only contain two status: 1 or 0.
//in fact, if you don't care the memory usage, you can just use a char array or int array to replace this bigsign.
//BTW, memory usage is often very large, because vtId may be not continuous. e.g. phone number: 13514992816(my chinese number,rui). if you use convert these num to continuous, it's ok. but if you not, you may need to think about the memory usage.
uint32_t *createBitSign(long vtMax);
int getBitSign(uint32_t *bitsign, long vtId);
//set bit to 1.
void setBitSign(uint32_t *bitsign, long vtId);
//clear bit to 0.
void clearBitSign(uint32_t *bitsign, long vtId);
void freeBitSign(uint32_t *bitsign);

#endif
