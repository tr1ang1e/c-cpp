
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

/* Function to handle users input and to put it into:
 *   > uint32_t type = default
 *   > int32_t type  = cast from uint32_t
 *					   cast is safety after checking if uint32_t value less or equal INT32_MAX
 * 
 * Function is insensitive to sizeof(unsigned long) value
 * Allowed range from -2147483648 to +4294967295 
 * 
 * Allowed input formats (autodetect):
 *   > dec: 1234, +1234, -1234
 *   > hex: 0xABCD (case insensitive both for 'x' symbol and value symbols)
 *   > oct: this format is permitted but might be simply turned oò
 * 
 * [!] 
 * If '-' is in string, strtoul() do this:
 *    unsigned long temp = [positive value after '-']
 *    do unary minus:  1 0 0 0 0...
 *	                     x x x x...
 *                     ^ 
 *	                   empty bit
 *    try to fit result value into (unsigned long) as is (not casting to signed, just by bits)
 *    if cant, set [value = UINT32_MAX] and [errno = ERANGE]
 * Thats why it is possible to have error "unsigned value overflow" even if signed value was given
 * 
 * */

int main(int argc, char** argv)
{
	const char* input = argv[1];
	const char* expectedEnd = &input[strlen(input)];
	uint32_t value = 0;
	int result = EXIT_FAILURE;

	do
	{
		//  check if input is in OCT format
		if (strlen(input) > 1 && input[0] == '0' && tolower(input[1]) != 'x')
		{
			printf(" :: error, oct value");
			break;
		}

		// read value
		char* actualEnd = NULL;
		value = strtoul(input, &actualEnd, 0);
		printf(" :: sizeof(unsigned long) = %zu \n", sizeof(unsigned long));

		// be sure all symbols are correct
		if (actualEnd != expectedEnd)
		{
			printf(" :: error, wrong symbol = '%c' \n", actualEnd[0]);
			break;
		}

		// check flow for sizeof(unsigned long) == 4
		if (sizeof(unsigned long) == 4)
		{
			// check if value fits into target uint32_t type   
			if (errno == ERANGE)
			{
				printf(" :: error, unsigned value overflow (sizeof(ul) is 4) \n");
				break;
			}

			// check if negative value was given and it fits into potential int32_t
			if (input[0] == '-' && ((value & (1 << 31)) == 0))
			{
				printf(" :: error, signed value overflow (sizeof(ul) is 4) \n");
				break;
			}
		}
		// check flow for sizeof(unsigned long) > 4
		else
		{
			// check if value fits into target uint32_t type 
			if (input[0] != '-' && strtoul(input, NULL, 0) > UINT32_MAX)
			{
				printf(" :: error, unsigned value overflow (sizeof(ul) is 8) \n");
				break;
			}

			// check if negative value was given and it fits into potential int32_t
			if(input[0] == '-' && strtol(input, NULL, 0) < INT32_MIN)
			{
				printf(" :: error, signed value overflow (sizeof(ul) is 8) \n");
				break;
			}
		}

		result = EXIT_SUCCESS;
		printf(" :: result, hex = 0x%08X, uns = %u, sig = %d", value, value, value);
	} while (0);
	
	return result;
}