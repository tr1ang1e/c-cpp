/* Alignment is used for simplify process of CPU gets data and make it faster
 * There are two ways to achieve alignment:
 *		- stack = var 1: padding (non-optimize compilation), var 2: change variables with their places (optimization)
 *      - structures = only padding is applied
 *
 * Alignment requirements:								
 *		1-byte types  >>  have not requirements (and their arrays as well >> 'char' and 'char[]')	
 *      2-byte types  >>  must start on even address				
 *      4-byte types  >>  must start on address divisible by 4		
 *      8-byte types  >>  must start on address divisible by 8
 * Sign'ess doesn't affect alignment
 *
 * For structures (not for stack) it's possible to avoid padding by using 
 * #pragma pack(push, N)	// N = number, which address must be divisible
 *	 struct S { ... };
 * #pragma pack(pop)
 * 
 * [!] terms
 * Padding = process of adding empty bytes to meet alignment requirements 
 * Packing = process of keeping structure with 'as is' size
 * 
 * */

#include <stdio.h>
#include <stdint.h>

int main(int argc, char** argv)
{
	/* __1__ : Alignment, stack variables swapping (compile with -01 or -02 flag)
	 *  
	 * Pay attention for declaration order vs. real addresses
	 *   decl.: a - b - c - d
	 *   addr.: a - d - b - c
	 * 
	 * */
	int  a = 0;
	char b = 0;
	char c = 0;
	int  d = 0;
	printf(" :: __1__ : Alignment, stack variables swapping\n");
	printf("    a = %p \n", &a);
	printf("    b = %p \n", &b);
	printf("    c = %p \n", &c);
	printf("    d = %p \n", &d);


	/* __2__ : Alignment, structure padding
	 *
	 * sizeof(uint8_t) + sizeof(uint32_t) = 5
	 * sizeof(struct S) = 8
	 * 
	 *    +---+---+---+---+---+---+---+---+
	 *    | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
	 *    +---+---+---+---+---+---+---+---+
	 *    | i | _padding_ |       j       |
	 *    +---+---+---+---+---+---+---+---+
	 * 
	 * */
	struct S
	{
		uint8_t  i;	    
		uint32_t j;	
	};
	printf(" :: __2__ : Alignment, struct padding  >>  sizeof(S) = %llu \n", sizeof(struct S));


	/* __3__ : Alignment, another rule for structure
	 * 
	 * Another rule for structure alignment is that 
	 * >> structure itself must have size, divisible 
	 *    by size of it's greatest member.
	 * 
	 * sizeof(uint64_t) + sizeof(uint8_t) = 9
	 * sizeof(struct T) = 16
	 * 
	 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 *    | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
	 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 *    |               j               | i |         _padding_         |
	 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 * 
	 * */
	struct T
	{
		uint64_t j;
		uint8_t  i;
	};
	printf(" :: __3__ : Alignment, another rule for structure  >>  sizeof(T) = %llu \n", sizeof(struct T));





	return 0;
}