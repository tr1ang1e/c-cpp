#include <stdio.h>
#include <stdint.h>

/*
 
	value bits common representation:          address space in common representation:			

	+----------+----------+                    +---+                	                        
	| .......8 | .......0 |                    |   | FF..FF                                    
	+----------+----------+                    +---+                                           
	|    H     |    L     |                     ...       big     lit endian                                      
	+----------+----------+                    +---+     +---+   +---+                                   
	     ^         ^                           |   |  >  | L |   | H |                                           
	     |    low byte = low bits (0..7)       +---+     +---+   +---+                                           
	     high byte = high bits (8..15)         |   |  >  | H |   | L |  <  address of full variable                                 
	                                           +---+     +---+   +---+              
	                                            ...     
	                                           +---+    
	                                           |   | 00..00
	                                           +---+ 
 
 */

int main()
{
	uint16_t x = 0x0001;
	printf("%s-endian\n", *((uint8_t*)&x) ? "little" : "big");

	return 0;
}