/* According to Eric S. Raymond: "The lost art of structure packing"
 * 
 * Alignment is used for simplify process of CPU gets data and make it faster
 * There are two ways to achieve alignment:
 *		- stack = var 1: padding (non-optimize compilation), var 2: change variables with their places (optimization)
 *      - structures = only padding is applied
 *
 * Alignment requirements								
 *		1-byte types  >>  have not requirements (and their arrays as well >> 'char' and 'char[]')	
 *      2-byte types  >>  must start on even address				
 *      4-byte types  >>  must start on address divisible by 4		
 *      8-byte types  >>  must start on address divisible by 8
 * Sign'ess doesn't affect alignment
 * 
 * [!] 
 * Terms
 * 		Padding = process of adding empty bytes to meet alignment requirements 
 * 		Packing = process of keeping structure with 'as is' size
 * 
 * [!]
 * Padding bytes are not guaranteed to be zeroes!
 * 
 * [!]
 * the presented requirements are commonly used and almost the standard >> the jargon for them is "self-alignment"
 * but be aware of possibility if different approach to the data alignment (especially in embedded systems)
 * 
 * */


#include <stdio.h>
#include <stdint.h>


int main(int argc, char** argv)
{
	printf("\n");
	
	/* __1__ : Alignment, stack variables swapping (compile with -01 or -02 flag)
	 *  
	 * Pay attention for declaration order vs. real addresses
	 *   decl.: a - b - c - d
	 *   addr.: a - d - b - c
	 * 
	 * [!]
	 * C standard doesn't specify the concrete rule to place variables in memory
	 * so result might differ from compiler/architecture to compiler/architecture
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
	printf("\n");

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
	printf(" :: __2__ : Alignment, structure padding  >>  sizeof(S) = %llu \n", sizeof(struct S));


	/* __3__ : Alignment, structure trailing padding
	 * 
	 * Another rules for structure alignment is that: 
	 *		>> structure itself must have size, divisible by size of it's greatest member
	 * so even if members order permits to avoid padding between them,
	 * trailing padding is necessary to meet structure alignment requirements
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
	printf("\n");


	/* __4__ : Structure instance address
	 * 
	 * The rules are: 
	 *   > structure instance has the same address as it's first member = no leading padding bytes
	 *   > structure must have alignment of it's widest member
	 * 
	 * The sequence of these rules is that entire structure as well as it's first member
	 * would be aligned according to the alignment requirement of it's widest member. For example:
	 * 
	 * 		As it was already said in __1__, char variable has no alignment requirements and might have any address.
	 * 		But if char variable is the first structure member, it must obey the rule of structure alignment, because
	 * 		structure itself must be aligned (see __3__). So structure with char as it's first member must has 
	 * 		address divisible by it's widest member = just like other types on the first member place.
	 * 
	 * [!]
	 * Unlike bit fields in structure, 
	 * 		- non-bit-field members and 
	 * 		- the units in which bit-fields reside 
	 * have addresses that INCREASE IN THE ORDER in which they are DECLARED 
	 * [C11, 6.7.2.1, 15]
	 * 
	 * */
	struct T instance = { 0 };
	printf(" :: __4__ : Structure instance address \n");
	printf("    structure address     =  %p \n", &instance);
	printf("    first member address  =  %p \n", &instance.j);

	// get offset of structure's member (e.g. offsetof(...) macro in <stddef.h>)
	#define OFFSET(T, member) (size_t)(&((struct T*)0)->member)	
	size_t offset = OFFSET(T, i);
	printf("    offset of second member is %zu \n", offset);
	printf("\n");


	/* __5__ : Nested structures alignment
	 * 
	 * Let us say we have two structures: struct Outer and struct Inner
	 *   - struct Inner alignment depends only on it's members
	 *   - struct Outer alignment dependd on widest member of both Outer and Inner structures
	 * 
	 * 
	 *    +---------------+---------------+-------------------------------+
	 *    |               |     Inner     |                               |
	 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 *    | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
	 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 *    |   w   | _pad_ | inner_member  |   x   |   y   | z |   _pad_   |
	 *    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	 *                ^                                           ^
	 *                to align Inner by 4                         to align Outer by 4 despite of 
	 *                                                            the most wired Outer member is 2
	 * 
	 * 
	 * Result size of struct Outer = 16 (not 14) because of inner_member of struct Inner
	 * 
	 * */
	struct Outer
	{
		uint16_t w;
		
		struct Inner
		{
			uint32_t inner_member;
		} nested_struct;

		uint16_t x;
		uint16_t y;
		uint8_t  z;
	};
	printf(" :: __5__ : Nested structures alignment \n");
	printf("    struct Inner size = %zu \n", sizeof(struct Inner));
	printf("    struct Outer size = %zu \n", sizeof(struct Outer));
	printf("\n");


	/* __6__ : Warning about types sizes
	 *
	 * C standard:
	 *   - provides only minimal sizes of int types (exept types in <stdint.h>)
	 *   - provides only minimal sizes of float types
	 *   - doesn't provide exactly size of enum type
	 * so padding in structures using these types might vary from realization to realization
	 * 
	 * */


	/* __7__ : Packing techniques: using #pragma pack
	 * 
	 * The most direct technique to avoid paddings is to use #pragma compiler directives:
	 *   > using for structures and unions
	 *   > valid for GCC and clang compilers (need to clarify about defferent)
	 * 
	 * First variant  =  #pragma pack( [n] )	
	 *	 > use alone for apply 'n' from now to the end 
	 *   > without 'n' default value will be applied
	 *   > 'n' means number for alignment rule (1, 2, 4, 8, 16 are available)
	 * 
	 * Second variant  =  #pragma pack( push [, id] [, n] )
	 *	                  #pragma pack( pop [, { id | n }] )
	 * 
	 *	 - 'push' = push to compiler stack, 'pop' = pop from compiler stack
	 *   -  'id' = possibility to coordinate 'push' and 'pop' to have random (not like stack) access
	 *   - 'pop, n' is the same as 'pop' + 'push, n' (replace current 'n' value)
	 * 
	 * 
	 * [!]
	 * This technique forces the generation of more expensive and slower code
	 * The only reason for #pragma pack is compatibility with to some kind of bit-level hardware or protocol requirement
	 * 
	 * */

	struct Nonpacked
	{
		uint8_t  a;
		uint32_t b;
		uint8_t  c;
	};

	#pragma pack(push, 1)
	struct Packed
	{
		uint8_t  a;
		uint32_t b;
		uint8_t  c;
	};
	#pragma pack(pop)

	printf(" :: __7__ : Packing techniques: using #pragma pack \n");
	printf("    struct Nonpacked size = %zu \n", sizeof(struct Nonpacked));
	printf("    struct Packed size = %zu \n", sizeof(struct Packed));
	printf("\n");


	/* __8__ : Packing techniques: reordering structure
	 *
	 * Another simple way is to place structure members in :
	 *   - decreasing order  >>  no padding bytes between members (possible trailing padding)
	 *   - increasing order  >>  no trailing padding (possible padding bytes between members)
	 * 
	 * [!] Be aware of code readability
	 * 
	 * [!] Be aware of cache locality. Comment:
	 * When CPU deal with data in memory it puts them firstly into cache lines (storage of 32 or 64 bytes size) 
	 * for reasons of perfomance increasing. The nearer data required in one particular context, the performance 
	 * is high - because of RAM access number reducing. That's why structure packing must be carefully: grouping 
	 * related and co-accessed data in adjacent fields. Moreover, sometimes the better performance might
	 * be achieved by transforming array of structures to structure of arrays.
	 * 
	 * */

	struct Nonreordered
	{
		char a;
		long long b;
		char c;
		int d;
		char e;
	};

	struct Decreasing  
	{
		long long a;
		int b;
		char c, d, e;
	};

	struct Increasing
	{
		char a, b, c;
		int d;
		long long e;
	};

	printf(" :: __8__ : Packing techniques: reordering structure \n");
	printf("    struct Nonreodered size = %zu \n", sizeof(struct Nonreordered));
	printf("    struct Decreasing size = %zu \n", sizeof(struct Decreasing));
	printf("    struct Increasing size = %zu \n", sizeof(struct Increasing));
	printf("\n");


	return 0;
}