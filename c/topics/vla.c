
/* VLA = variable-length array
 * 
 * Type description:
 *   - VLA size is known only in run time (declarator is not an integer_constant_expression (see docs))
 *   - memory for VLA is allocated on stack and doesn't require free() call
 *   - exists only in the scope of the declaration
 *   - cannot be a part of structure or union
 *   - have hidden variable to store the size value wich might be associated with several VLA
 *   - { } initializers are forbidden for VLA 
 *         
 * */

#include <stdio.h>

size_t side_effect()
{
	return printf("  a = side_effect() \n");
}

void to_regular(int array[5][5])
{
	printf("  b = VLA >> to_regular() \n");
}

void to_vla(size_t a, size_t b, int VLA[a][b])
{
	printf("  b = regular >> to_vla() \n");
}

int main(int argc, char** argv)
{
	/* __1__ : Critical code
	 *
	 * When VLA is mentioned in code, this line:
	 *   a. if there is side effect while VLA size evaluatuion is possible, it will appear
	 *   b. must be executed to evaluate VLA size  >>  couldn't be skipped by 'goto' operator
	 *
	 * */

	printf("\n");
	printf(" :: __1__ : Critical code \n");

	// a
	int VLA1[ side_effect() ];
	printf("\n");


	// b  >>  uncomment goto and skip label lines to get compile error
	size_t size = 100;
	// goto skip;
		typedef int myvla[size];		// even typedef with VLA is enough to create critical code
	// skip:


	/* __2__ : Regular array compatibility
	 * 
	 * As a VLA value is stored separately of the VLA object
	 * and doesn't the part of the VLA type itself, 
	 * VA and arrays of constant known size are compatible
	 * 
	 * There are two-dimensions arrays to avoid array to pointer
	 * resucing during a function call
	 * 
	 * */

	printf(" :: __2__ : Regular array compatibility \n");

	// regular array to function VLA argument
	int array[5][5];
	to_vla(5, 5, array);
	
	// VLA to function regular array argument
	size_t n = 5;
	int VLA2[n][n];
	to_regular(VLA2);

	// compatibility on the pointer expample (correct if sizes are the same >> programmer's responsibility) 
	int(*ptr)[5][5] = &VLA2;		
	int (*ptrVLA)[n][n] = &array;


	printf("\n");
	return 0;
}
