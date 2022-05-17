
/* Every number is presented in the common bits format
 * following examples would use 8-bit format for conviniency 
 * 
 * example value for explaining = 10011001
 * real values = depends on code
 * 
 * */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// sign extension for negative = 1 << (liftmostBitIdx + 1) - value
// sign extension for positive = nothing to do

// 1011 = deside what we are dealing with:
//   > representation : the same							
//   > interpretation : different >> change sign (unary minus) or interpret as (un)signed
//		signed = -5					 1 << (sizeInBits) - value    get twos-complement
//		unsigned = 11

void print_bits(void* value, size_t bits)
{
	int mask = 1;
	char result[bits + 1] = { 0 };

	for (size_t i = bits - 1; i >= 0; --i)
	{

	}

}

int main()
{
	/* __1__ : SIGNED and UNSIGNED values
	 * 
	 * value of variable is always the same
	 * unsigned = 10011001
	 * signed   = 10011001
	 *
	 * for the processor they are completely the SAME
	 * the difference only on level of interpretation by language  language level
	 *            
	 * 
	 * unsigned = 10011001 
	 *            ^^^^^^^^
	 *            every bit in value is 2^bitIndex
	 *
	 * */
	
	
	// uint32_t var = strtoul("111", NULL, 0);

	signed char s = -1;
	unsigned char u = 1;

	if (s < u)
	{
		printf(" :: \n");
	}
	
	
	return 0;
}