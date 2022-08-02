
/* INTEGER TYPES and CONVERSIONS
 *
 * Standard of C language defines two hierarchies of integer types:
 *   - integer sizes
 *   - integer conversion ranks
 *
 * Conversions:
 *   - integer promotion
 *   - usual arithmetic conversions
 *
 * */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>


int main(int args, char** argv)
{

	/* __1__ : integer sizes
	 *
	 * Integer types in C:
	 *   - basic = char, int
	 *   - additional = enumerations, _Bool
	 *
	 * For basic integer types there are two modifiers types:
	 *   > size = short, long          >>  for int only / might be used without int (e.g. short = short int)
	 * 	 > signess = signed, unsigned  >>  signed is default (e.g.: int = signed int)
	 *                                     exception: char has implementation defined signess
	 *
	 * Standard of C language doesn't provide exact bits sizes of integer types,
	 * but it gives the not-less-then rule. The sequence is:
	 *
	 * type listing		char <= short <= int <= long <= long long
	 * min bit size       8       16      16      32      64
	 * sizeof(type)       1
	 *
	 * - char = smallest addressable type, size of char in bits = CHAR_BIT (<limit.h>) = usualy 8 bits
	 * - signed and unsigned types have the same bits size
	 * - the fixed int sizes are in <stdint.h> and have name template <signess>int<bits>_t, e.g.:
	 *     uint8_t = unsignet integer with 8 bits size
	 *     int16_t = signed integer with 16 bits size
	 *
	 * - enumerations have 'int' type and therefore depends on implementation defined 'int' size
	 * - _Bool also has 'int' type also it might get hust zero or one values
	 *
	 * */

	printf("  \n");
	printf(" :: __1__ : integer sizes \n");
	printf(" sizes of integers types for given compiler (bits): \n");
	printf("     char = %zu \n", sizeof(char) * CHAR_BIT);
	printf("    short = %zu \n", sizeof(short) * CHAR_BIT);
	printf("      int = %zu \n", sizeof(int) * CHAR_BIT);
	printf("     long = %zu \n", sizeof(long) * CHAR_BIT);
	printf("    llong = %zu \n", sizeof(long long) * CHAR_BIT);
	printf("  \n");



	/* __2__ : integer conversions ranks
	 *
	 * Conversion rank determines how conversions are performed. Different
	 * integer types might have the same representation, but not coversion ranks.
	 *
	 * Following rules describe how do types convert in both:
	 *	  - implicit = when non-compatible types are operands of an arithmetic operator
	 *   - explicit = cast from type to another
	 *
	 * [!]
	 * Conversion of an operand value to a compatible type causes
	 *   - no change to the  >>  value
	 *   - no change to the  >>  representation
	 *
	 * Rules, see C__N1570__6.3.1.1:
	 *
	 *         _Bool << char << short << (int, enum) << long << long long
	 *
	 * - sign T and unsign T have the same rank
	 * - char and (un)signed char have the same rank
	 *
	 * */

	printf(" :: __2__ : integer conversion ranks \n");
	printf("    _Bool << char << short << (int, enum) << long << long long \n");
	printf("  \n");



	/* __3__ : integer promotion
	 *
	 * For the following categories:
	 *   - int types with conversion rank less_than or equal_to (un)signed int
	 *   - bit-field of type _Bool or (un)signed int
	 * there is the rule:
	 *
	 * In every expression when (un)signed int might be used,
	 * these types are implicitly converted to:
	 *   >   signed int  >>  if signed int can represent all values of the original type
	 *   > unsigned int  >>	 otherwise
	 *
	 * See exaples of this rule consequences (sometimes they are not intuitive)
	 *
	 * [!]
	 * Exception! Operators like ++ oe sizeof() don't lead to integer promotion,
	 * cause there is no sence to deal with any types in such a way
	 *
	 * [!]
	 * Integer constant is also considered as of int type, e.g:
	 * long long var = 1; is the same as { int i = 1; long long var = (long long)i; }
	 *
	 * */

	printf(" :: __3__ : integer promotion \n");

	// example 1 : bitwise operations
	unsigned char a = 1;
	unsigned char b = ~a >> 1;		// intuitive: ~00000001 >> 1 = 11111110 >> 1 = 01111111 = 0x7F or 127
	// actual ('a' integer promotion to int): ~00..00000001 >> 1 = 11..11111110 >> 1 = 01..11111111 = 255 
	printf("    example 1, bitwise operations. Expected = %d. Actual = %hhu\n", 0x7F, b);

	// example 2 : char comparison
	signed char i = 0xFC;			// signed   11111011 promoted to int = 11..11111011 (sign extension so value is saved)  
	unsigned char j = 0xFC;			// unsigned 11111011 promoted to int = 00..11111011 (do nothing just fill bits witn 0)
	const char* isEqual = (i == j) ? "equal" : "not equal";
	printf("    example 2, char comparison. Chars: i is '%c', j is '%c'. They are [ %s ] \n", i, j, isEqual);

	// example 3 : exception
	j = 255;  // if integer promotion was applied the ++j result value would be 256
	printf("    example 3, exception. Original value is 255. ++255 will behave as expected = %d \n", ++j);
	printf("  \n");



	/* __4__ : usual arithmetic conversions
	 *
	 * Topic relevant types overview, C__N1570__6.2.5:
	 *
	 *     +-------------------------------------------------------------------------------------------------------------------+
	 *     |                                                 aritmetic types                                                   |
	 *     +---------------------------------------------------------+---------------------------------------------------------+
	 *     |                      integer types                      |                     floating types                      |
	 *     +----------------------------+----------------------------+----------------------------+----------------------------+
	 *     |    basic integer types     |   extended integer types   |    real floating types     |   complex floating types   |
	 *     +----------------------------+----------------------------+----------------------------+----------------------------+
	 *     ^                                                                                      ^                            ^
	 *     + -- -- -- -- -- -- -- -- -- -- --  real type domain  -- -- -- -- -- -- -- -- -- -- -- + --  complex type domain -- +
	 *
	 *   > basic integer types			=  standart signed integer types + standart unsigned integer types
	 *   > extended integer types	    =  additional implementation-defined (signed + unsigned) integer types
	 *   > real floating types			=  float, double, long double
	 *   > complex floating types		=  float _Compex, double _Complex, long double _Complex
	 *
	 *
	 *
	 * General purpose of usual arithmetic conversions is to determine common-real-type
	 * for operands of given arithmetic operator and therefore of the final result. In this
	 * document the information will be only about aritmetic expressions of ...
	 *   ... integer types subset of ...
	 *   ... real type domain
	 *
	 * Conversions are done in 2 steps, C__N1570__6.3.1.8:
	 *   1 = integer promotion if necessary
	 *   2 = conversions according to the rules in the following order :
	 *		 a. if types are same - no conversions
	 *       b. otherwice, if both operands are signed or unsigned - to type with higher conversion rank
	 *       c. otherwise, if unsigned operand has greater than or equal to rank of signed operand - to unsigned operand type
	 *       d. otherwise, if signed operand can represent all of unsigned operands possible values - to sidned operand type
	 *       e. otherwise, - to unsigned type corresponding to the type of signed operand
	 *
	 * */

	printf(" :: __4__ : usual arithmetic conversions \n");
	printf("    a. signed int + signed int               >>  signed int     \n");
	printf("    b. signed int + signed long              >>  signed long    \n");
	printf("    c. signed int + unsigned long            >>  unsigned long  \n");
	printf("    d. signed long (64) + unsigned int (32)  >>  signed long    \n");
	printf("    e. signed long (32) + unsigned int (32)  >>  unsigned long  \n");
	printf("  \n");

	return 0;

}