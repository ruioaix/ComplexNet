#include "../inc/bitsign.h"

uint32_t *createBitSign(long vtMax)
{
	uint32_t *bitsign=calloc(WORD_OFFSET(vtMax)+1, sizeof(uint32_t));
	memError(bitsign, "createBitSign bitsign");
	return bitsign;
}

int getBitSign(uint32_t *bitsign, long vtId)
{
	int bit = bitsign[WORD_OFFSET(vtId)] & (1 << BIT_OFFSET(vtId));
	return bit != 0; 
}

void setBitSign(uint32_t *bitsign, long vtId)
{
	bitsign[WORD_OFFSET(vtId)] |= (1 << BIT_OFFSET(vtId));
}

void clearBitSign(uint32_t *bitsign, long vtId)
{
	bitsign[WORD_OFFSET(vtId)] &= ~(1 << BIT_OFFSET(vtId));
}

void freeBitSign(uint32_t *bitsign)
{
	free(bitsign);
}
