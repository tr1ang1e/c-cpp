/* When using macros it is possible to:
 * 
 *   # = stringification = transform macro argument to string
 *  ## = concatenation = connect two parts together to have new identificator
 * 
 * */

#include <stdio.h>

// stringification
#define TOSTRING(arg) (#arg)

// concatenation
#define NEWIDENT(arg) _var_##arg

int main(int argc, char** argv)
{
	int var = 42;
	printf(" :: __  # __ : variable value = %d, variable name = %s \n", var, TOSTRING(var));

	int NEWIDENT(0) = 0;
	int NEWIDENT(1) = 1;
	printf(" :: __ ## __ : autonamed variables' values = %d, %d \n", _var_0, _var_1);

	return 0;
}