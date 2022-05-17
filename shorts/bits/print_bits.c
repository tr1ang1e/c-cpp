#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <malloc.h>

#define PVOID(arg) ((void*)&arg)    
#define BITS(type) (sizeof(type) * 8)

void print_bits(void* value, size_t bits)
{
	/*

		b = 1 byte
		u = 4 bytes = uint32_t

		STACK, main

		+---+ --+
		| b |   |
		+---+   |
		| b |   |
		+---+    > uint32_t i = ... ;
		| b |   |
		+---+   |
		| b |   |  == &i
		+---+---+
		| b |   |
		+---+   |
		| b |   |
		+---+    > uint32_t j = ... ;
		| b |   |
		+---+   |
		| b |   |  == &j
		+---+ --+
		  |
		  V
	 grows down


		CAST, function
		uint64_t var = *(uint64_t*)(&b)

			  +---+---+   +---+---+---+---+---+---+---+---+
		var = | u | u | = | b | b | b | b | b | b | b | b |
			  +---+---+   +---+---+---+---+---+---+---+---+
						  ^------ i ------^------ j ------^
										   ^
										   mask 10..00 must be here

		so mask to print 'j' bits must be = 1 shifted left for (sizeof(j) - 1)
		uint64_t mask = (uint64_t)1 << (sizeof(uint32_t) - 1)

	 * */

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

int main(int argc, char** argv)
{
	int32_t s = -11;
	uint32_t u = 5;

	print_bits(PVOID(s), BITS(int32_t));
	print_bits(PVOID(u), BITS(uint32_t));

	return 0;
}