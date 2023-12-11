#include <stdio.h>
#include <stdint.h>


/* CRC might be calculated:
 *		directly = checking data bit by bit
 *      using special table = cheking several bits in a time
 * 
 * The second approach is introduced here
 *      size of table element must be the same as poly size
 *      table size depends on size of incoming data, formula:
 *		    size = 2^D, 
 *		    D = number of simultaneously calculated bits
 *          in current example bit number is 8, so size is 256
 *
 * */
#define BITS_NUMBER   8
#define TABLE_SIZE	256


// One of possible table generation algorythms
void generate_crc_table(const uint32_t poly, uint32_t* table)
{
    for (uint32_t entry = 0; entry < TABLE_SIZE; entry++)
    {
        uint32_t result = entry;

        // usually starts with 1 entry so 0 entry is 0x00000000
        // matters only is second side has the same table or not
        for (uint32_t bit = 0; bit < BITS_NUMBER; bit++)        
        {
            if ((result & 1) == 1)
            {
                result = (result >> 1) ^ poly;
            }
            else
            {
                result = result >> 1;
            }
        }
        table[entry] = result;
    }
}

uint32_t calculate_crc32(uint32_t* table, uint8_t* data, size_t dataSize)
{
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < dataSize; ++i)
    {
        crc = (crc >> 8) ^ table[data[i] ^ (crc & 0xFF)];
    }

    crc ^= 0xFFFFFFFF;

	return crc;
}

int main(int argc, char** argv)
{
	const uint32_t poly = 0xEDB88320;
	uint32_t table[TABLE_SIZE] = { 0 };
	generate_crc_table(poly, table);

    /* unkomment to see generated table 
    * 
        for (size_t entry = 1; entry < TABLE_SIZE + 1; ++entry)     // convinient print form, new line when devisible by 8
        {
            printf("0x%08x, ", table[entry]);
        
            _Bool newLine = !(entry % 8);
            if (newLine)
            {
                printf("\n");
            }
        }
     *
     * */

    uint8_t data[] = { 0x00, 0x01, 0x02, 0x03 };
    uint32_t crc = calculate_crc32(table, data, sizeof(data));
    printf("CRC = 0x%08X \n", crc);
    
	return 0;
}