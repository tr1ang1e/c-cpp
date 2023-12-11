/* Bit fields are used to
 *   - simplify access to bits inside value
 *   - sparingly pack values into memory
 * Bit fileds are used as regular types but with less number of bits 
 * 
 * The bit field storage object called 'allocation unit'
 * 
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |                unit               |                unit               |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |.....|.|....|....|....|.......|........|....|....|...........|.|...|...|   >>  bit fields and padding bits (if required)
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * 
 * */
 
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>


/* __1__ : Known
 * 
 * A. declaration:
 *    - might be of types described in example
 *    - are limited by declared type bit size
 *    - might be declared with 'const' and 'volatile' specificators
 * 
 * B. usage:
 *    - fit value according to number of bit and to signess
 *    - cannot be used with _Alignas() specifier
 *    - cannot be used with sizeof() and '&' (take address) operators
 *    - unsigned bit filed never overflows
 *      ^
 *      if overflow then result = reduced modulo of 2^bits
 *              value = 29		11101
 *  unsigned int field : 3        ...   
 *              result = 5		  101
 * 
 * */

struct bits
{
	unsigned int i : 3;		// unsigned, never overflows 
	  signed int j : 3;     // signed
	         int x : 3;     // implementation-defined signedness
		   _Bool b : 1;		// bool, only single bit is allowed

      unsigned int : 3;		// any type, anonimous = explicit padding bits
	  unsigned int : 0;		// any type, zero value = next bit filed begins at the beggining of the next unit
};


/* __2__ : Unknown
 *	
 * A. implementation-defined
 *     - type      >>  is allowable to be other type then specified in __1__
 *	   - sign      >>  sign of plain 'int' bit-field (plain = not explicitly 'signed' or 'unsigned')
 *     - atomic    >>  whether bit filed is allowd to be atomic
 *     - pack      >>  bit fields permitted (but not necessarily) to be packed together
 *     - order     >>  order of bit fields within an allocation unit
 *     - straddle  >>  whether a bit field can straddle an allocation unit boundary
 * 
 * B. unspecified  >>  alignment of the allocation unit that holds a bit field
 * C. undefined    >>  the effect of calling offsetof() on a bit field
 *  
 * */


/* __3__ : Usage with union
 *
 * Union usage with fields is convinient in purpose of setting values 
 * not separately for every of bit fields but with one command per union
 * 
 * [!]
 * As it already have been mentioned, there are a lot of implementation
 * defined or unspecified points about bitfields. So the following example
 * just demonstrates the (union + bit fields) implementation for comliper
 * that was used for writing this article and might be not valid
 * for different compiler. General rules about it are:
 *    - use bit fields across different compilers only in specified ways
 *    - within one project know compiler rules before using bit fileds in non-specified ways 
 * 
 */

// utility
#define PVOID(arg) ((void*)&arg)    
#define BITS(type) (sizeof(type) * 8)
static void print_bits(void* value, size_t bits);

struct S
{
	union
	{
		uint16_t var;
		struct
		{
			uint16_t i : 11;
			uint16_t j : 5;
		};

	};
};

int main(int argc, char** argv)
{
	struct S s = { 0 };
	s.var = 0x0801;

	// result
	printf("\n");
	printf(" :: __3__ : Usage with union \n");
	printf("    sizeof(struct S) = %zu \n", sizeof(struct S));
	printf("    s.var = 0x%04X \n", s.var);
	printf("    s.i = 0x%x \n", s.i);
	printf("    s.j = 0x%x \n", s.j);

	// visualization
	printf("\n");
	uint8_t high_byte = *(((uint8_t*)&s.var) + 1);
	uint8_t low_byte = *(uint8_t*)&s.var;
	printf("            +- %02X -++- %02X -+ \n", high_byte, low_byte);
	printf("            |      ||      | \n");
	print_bits(PVOID(s.var), BITS(uint16_t));
	printf("            |   ||         | \n");
	printf("            + j ++--- i ---+ \n");
	printf("\n");

	return 0;
}

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

	printf("    s.var = %s \n", result);
	free(result);
}