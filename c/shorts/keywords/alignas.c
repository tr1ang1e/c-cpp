#include <stdio.h>
#include <stdint.h>


int main(int argc, char** argv)
{

	/* __1__ : Set alignment
	 *
	 * _Alignas = type specifiers to modify the alignment requirement of the object being declared
	 *   _Alignas( expression )  = particular alignment (if zero then no effect)
	 *   _Alignas( type )		 = alignment as of given type
	 * 
	 * #define alignas _Alignas
	 * in <stdalign.h> header (convinient form)
	 * 
	 * Rules:
	 *   > might do alignment stricter but not weaker (if several _Aligness() in one declaration  >>  strictest would be chosen)
	 *   > only needs to appear on the definition, might be used in declaration (UB if values are differrent in declaration and definition)
	 *   > might not be used for:
	 *     - function parameters declaration (only on the definition)
	 *     - struct declaration (only on the definition)
	 *     - struct with bit fileds
	 *     - object with 'register' specifier
	 *     - typedef
	 *
	 * */
	
	struct Noalignas
	{
		/* 4 */ uint32_t a;
		/* 4 */ uint32_t b;
		/* 5 */ uint8_t c[5];
		/* 3 = trailing padding, because of greatest alignment*/ 
	};

	struct Alignas
	{
		/* 4 */ uint32_t a;
		/* 4 = padding, because of following _Alignas(8) */
		/* 4 */ _Alignas(8) uint32_t b;
		/* 5 */ uint8_t c[5];
		/* 7 = trailing padding, because of greatest alignment */ 
	};

	printf("\n");
	printf(" :: __1__ : Set alignment \n");
	printf("    struct Noalignas size = %zu \n", sizeof(struct Noalignas));	
	printf("    struct Alignas size = %zu \n", sizeof(struct Alignas));
	printf("\n");


	/* __2__ :  Get alignment
	 * 
	 * _Alignof( type ) = returns the alignment requirement of given type
	 * 
	  #define alignof _Alignof
	 * in <stdalign.h> header (convinient form)
	 * 
	 * Rules:
	 *   > result is of size_t type
	 *   > if type is array result is alignment requirement of single element type
	 *   > operand is not evaluated, sizeof(VLA) is not evaluated
	 *   > might not be used for:
	 *     - incomplete type
	 *     - function
	 * 	 
	 * */

	printf(" :: __2__ : Get alignment \n");
	printf("    alignment requirement of struct Notalignas = %zu \n", _Alignof(struct Noalignas));
	printf("    alignment requirement of struct Alignas = %zu \n", _Alignof(struct Alignas));
	printf("\n");

	return 0;
}