
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

}