#include "../../inc/utility/bitsign.h"
#include <stdlib.h>
#include <assert.h>

unsigned int *createBitSign(int signMax)
{
	unsigned int *signStick=calloc(WORD_OFFSET(signMax)+1, sizeof(unsigned int));
	assert(signStick != NULL);
	return signStick;
}

int getBitSign(unsigned int *signStick, int signId)
{
	unsigned int bit = signStick[WORD_OFFSET(signId)] & (1 << BIT_OFFSET(signId));
	return bit != 0; 
}

void setBitSign(unsigned int *signStick, int signId)
{
	signStick[WORD_OFFSET(signId)] |= (1 << BIT_OFFSET(signId));
}

void clearBitSign(unsigned int *signStick, int signId)
{
	signStick[WORD_OFFSET(signId)] &= ~(1 << BIT_OFFSET(signId));
}
