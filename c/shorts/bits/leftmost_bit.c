#include <stdio.h>
#include <stdint.h>

// argument must be unsigned because right shifting of signed type is implementation dependent 
//		1. signed 1100 >>= 2 is signed 0011 (sign bit is shifted)
//      2. signed 1100 >>= 2 is signed 1001 (sign bit is fixed)
unsigned int get_leftmost_bit_index(unsigned int value)
{
	unsigned int result = 0;

	for (unsigned int idx = 0; idx < sizeof(unsigned int) * 8; ++idx)
	{
		if (value & 1)
		{
			result = idx;
		}

		value >>= 1;
	}

    return result;
}

int main(int argc, char** argv)
{
    unsigned int idx, value = 16;

    idx = get_leftmost_bit_index(value);

    printf(" :: The leftmost bit of 0x%X value is %d \n", value, idx);
}