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
 * |.....|.|....|....|....|.......|........|....|....|...........|.|...|...|   >>  bit fields (or padding bits)
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * 
 * */
 
#include <stdio.h>
#include <stdint.h>


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
 *		- sign      >>  sign of plain 'int' bit-field (plain = not explicitly 'signed' or 'unsigned')
 *     - atomic    >>  whether bit filed is allowd to be atomic
 *     - pack      >>  bit fields permitted (but not necessarily) to be packed together
 *     - order     >>  order of bit fields within an allocation unit
 *     - straddle  >>  whether a bit field can straddle an allocation unit boundary
 * 
 * B. unspecified  >>  alignment of the allocation unit that holds a bit field
 * C. undefined    >>  the effect of calling offsetof() on a bit field
 * 
 * 
 *  
 * Standart doesn't define "alignment of the addressable storage unit allocated to hold a bit-field"
 * So implementation-defined points are:
 *   - [!] the order of allocation of bit-fields within a unit
 *   — [!] are thread-safe (whether atomic types are permitted for bit-fields) 
 *   - whether a bit-field can straddle a storage-unit boundary
 *   — the alignment of non-bit-field members of structures
 * "This should present no problem unless binary data written by one implementation is read by another."
 * 
 * */


/* __3__ : Usage with union
 *
 * 
 * 
 * 
 * 
 */

struct S
{

};

int main(int argc, char** argv)
{


	return 0;
}