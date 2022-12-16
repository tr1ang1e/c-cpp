#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define PVOID(arg) ((void*)&arg)    
#define BITS(type) (sizeof(type) * 8)

void print_bits(void* value, size_t bits)
{
    // uint64_t is the greatest guaranteed size in standard 
	uint64_t mask = (uint64_t)1 << (bits - 1);
	uint64_t numb = *(uint64_t*)value;

	// '+1' to keep null - termanator
	char* result = calloc(bits + 1, 1);
	result[bits] = '\0';

	for (size_t i = 0; i < bits; ++i)
	{
		bool b = mask & numb;
		result[i] = (b ? '1' : '0');
		numb = numb << 1;
	}

	printf(" :: bits = %s \n", result);
	free(result);
}


int main()
{
	/* __1__ : SIGNED and UNSIGNED values in CPU
	 * 
	 * value of variable is always the same
	 * unsigned = 11100111
	 * signed   = 11100111
	 *
	 * For CPU they are completely the SAME = CPU doesn't know anything about sign'ess. The difference only 
	 * on level of interpretation by language level and the language is responsible for dealing with sign'ess 
	 *            
	 * unsigned = 11100111 
	 *            ^^^^^^^^
	 *            every bit in value is 2^bitIndex (2^0, 2^1, ...)
	 * 
	 * signed   = 11100111
	 *            |^^^^^^^ - value bits
	 *            ^    
	 *            sign bit
	 * 
	 * Following example will illustrate it. Explanation of why 232 and -24 are the same on
	 * the bits level would be presented in __2__ part
	 * 
	 * */
	printf(" :: __1__ : SIGNED and UNSIGNED values \n");

	uint8_t uns = 231;
	int8_t sig = -25;
	printf(" :: uns value = %hhu", uns); print_bits(PVOID(uns), BITS(uint8_t));
	printf(" :: sig value = %hhd", sig); print_bits(PVOID(sig), BITS(int8_t));
	
	/* __2__ : SIGNED and UNSIGNED values in LANGUAGE
	 *
	 * Let us take following 8-bit value
	 *   value = 25
	 *   bits  = 00011001
	 * Is it signed or unsigned value? According to the __1__ part we know, 
	 * that this is can't be established on grounds of bits representation
	 * 
	 * Let us decide that this value is SIGNED. As we already know,	
	 * the most significant bit of value is the sign bit. So we have:
	 *    most significant bit = 0_______ = '+' sign		 | result value
	 *    value bits           = _0011001 = 1 + 8 + 16 = 25  | is +25
	 *  
	 * Because of sign value is positive:
	 * - we can see that value of signed and unsigned are identic
	 * - we can safely cast signed to unsigned
	 * 
	 *			    BITS		VALUE	
	 *   SIGNED     00011001	+25
	 * UNSIGNED		00011001	+25
	 * 
	 * * * HOW TO GET -25 FROM +25 ? * * *
	 * 
	 * There are three ways to get negative value
	 * equal by module to given positive value
	 * 
	 *								OPERATION		    POSITIVE    NEGATIVE
	 * 1. just set sign bit to 1	value | (1 << 7)	00011001    10011001 = -25
	 * 2. ones' complement			~value				00011001    11100110 = -25
	 * 3. two's complement			~value + 1          00011001    11100111 = -25
	 *   
	 * To get decimal representation:
	 *    - do reverse (1, 2 or 3) operation	NEGATIVE >> 00011001
	 *    - count resulting positive value      0001011 = 1 + 2 + 8 = 25
	 *    - add '-' befor value					25 >> -25
	 * 
	 * C standard DOESN'T define the exact way for how language implementation
	 * must keep negative signed values inside memory and how to deal with them
	 * 
	 * * * WHAT IF CAST -25 to UNSIGNED TYPE ? * * * 
	 * 
	 * Compiler (CLANG 13.0.1) is using during this code compilation
	 * implements two's commplement representation. Let us see what would happen
	 * if just cast negative signed value to unsigned (obviously positive)
	 * 
	 * Nothing will be changed on bit value
	 * but interpretation would be completely different
	 * 
	 *			    BITS		VALUE	
	 *   SIGNED     11100111	= '-' and 00011001 = '-' (1 + 8 + 16) =  -25  | that's why signet negative and unsigned
	 * UNSIGNED		11100111	= 1 + 2 + 4 + 0 + 0 + 32 + 64 + 128   =  231  | values bits representations are the same
	 * 
	 * */

	/* __3__ : UNARY MINUS operator
	 * 
	 * Pay attention for the next point. As we already said, 
	 * C standard doesn't define negative values representation
	 * in signed types.
	 * 
	 * There is unary minus operator '-' which negates
	 * given positive value and guarantees having negative value
	 * in particular language implementation
	 * 
	 */
	printf(" :: __3__ : UNARY MINUS operator \n");

	int8_t pos = 42;
	int8_t neg = -pos;
	printf(" :: pos value =  %hhd \n", pos); 
	printf(" :: neg value = %hhd = -(pos)  \n", neg);

	/* __4__ : SIGN EXTENSION (does it works in the same way with not twos-compliment?)
	 *
	 * Sometimes it is necessary to represent M-bit signed value
	 * as N-bit signed value, and N > M. For example:
	 *    signed m = 0000xxxx = 4 bits
	 *    signed n = ________ = 8 bits
	 * 
	 *    [A]
	 * 
	 * There are no problems with positive values, because sign bit 
	 * set to 0, so it is enough to do: n = m
	 *    signed m     = 00000101 = 4 bits, sign bit is 0
	 *    signed n = m = 00000101 
	 * 
	 *    [B]
	 * 
	 * The are no problem with negative values as well,
	 * if types are built-in:
	 *    int8_t k = -5 = 8 bits
	 *    int16_t l = k = 16 bits
	 * such operations execute automatically when 
	 * 
	 *    [C]
	 * 
	 * But when they are about not standard bit-sizes,
	 * special operation is required. 
	 *    signed i = 00001xxx = 4 bits (1 sign + 3 value)
	 *    signed j = ________ = 8 bits (1 sign + 7 value)
	 *    result j = 11111xxx = called sign extension
	 *
	 * For example, problem might be solved using bit masks and bit operations.
	 * But the simpliest way is to make substraction according to the rule:
	 *    j = i - (1 << bitsnum);
	 *    bitsnum = number of bits in i, which represent value 
	 *    in example above, j = i - (1 << 4)
	 * Result for 1011:
	 *     0 0 0 0 1 0 1 1    |     0 0 0 0 1 0 1 1   |   (1) 1 1 1 10 1 0 1 1
	 *   - 0 0 0 1 0 0 0 0     >  - 0 0 0 1 0 0 0 0    >    - 0 0 0  1 0 0 0 0
	 *     - - - - - - - -    |     - - - - 1 0 1 1   |       1 1 1  1 1 0 1 1
	 * 
	 * */
	printf(" :: __4__ : SIGN EXTENSION \n");

	int8_t m = 5;
	int8_t n = m;
	printf(" ::   [A] : positive value \n");
	printf(" :: 4-bit m = %hhd", m); print_bits(PVOID(m), BITS(int8_t));
	printf(" :: 8-bit n = %hhd", n); print_bits(PVOID(n), BITS(int8_t));

	int8_t k = -5;
	int16_t l = k;
	printf(" ::   [B] : negative value, built-in types \n");
	printf(" :: 8-bit  k = %hhd", k); print_bits(PVOID(k), BITS(int8_t));
	printf(" :: 16-bit l = %hd" , l); print_bits(PVOID(l), BITS(int16_t));

	int8_t i = 11;
	int8_t j = i - (1 << 4);
	printf(" ::   [C] : negative value, custom bits number \n");
	printf(" :: 8-bit i = -5 (manually entered)"); print_bits(PVOID(i), BITS(int8_t));  // 5 == -11 in bits representation (like as 231 == -25)
	printf(" :: 8-bit j = %hd  (auto calculated)", j); print_bits(PVOID(j), BITS(int8_t));



	return 0;
}