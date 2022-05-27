/* Bit fields are used to
 *   - simplify access to bits inside value
 *   - sparingly pack values into memory
 * 
 * Bit fields:
 *   > are used as regular (8*N bit) types 
 *   > might be both signed and unsigned types
 *   > if of unsigned type would never overflow
 *       reduced modulo of 2^bit_field_size
 *       value = 29			 11101
 *       uint8_t field : 3     ___
 *       result = 5		       101
 *  
 * Standart doesn't define "alignment of the addressable storage unit allocated to hold a bit-field"
 * So implementation-defined points are:
 *   — whether a 'plain' int bit-field (char, short, int, long = which are not explicitely 'signed' or 'unsigned') is actually signed or unsigned 
 *   - allowable bit-field types other than _Bool, signed int, and unsigned int
 *   — whether atomic types are permitted for bit-fields
 *   - whether a bit-field can straddle a storage-unit boundary
 *   - [!] the order of allocation of bit-fields within a unit
 *   — the alignment of non-bit-field members of structures
 * "This should present no problem unless binary data written by one implementation is read by another."
 * 
 * */

#include <stdio.h>
#include <stdint.h>

typedef struct bits
{
	uint8_t x : 1;
	 int8_t y : 5;
	uint8_t z : 2;
} bits;

int main(int argc, char** argv)
{
	bits b = { 0 };
	b.x =  1;
	b.y = -3;
	b.z =  3;

	printf(" x = %hhu, y = %hhd, z = %hhu \n", b.x, b.y, b.z);

	return 0;
}