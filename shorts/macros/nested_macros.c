/* Algorythm of nested macro expanding:
 *   1. all macros are recognized in line
 *   2. repeat step 1. with expanded result
 *   3. not expanding macro if:
 *      - meet # or ## right befor it while expanding previous macro
 *      - this macro was already expand (prevent recursion)
 * 
 * */ 
 

#include <stdio.h>


/* __1__ : expand all nested macros
 *  
 * 1 = D(foo, bar)  >>  B( C(foo, bar) )
 *
 * 2 = B( C(foo, bar) )  >>  expand B and C then, cause they are in one line at the same time
 *     |   ^ 
 *     |   2.2 = C  >>  foo ## bar  >>  foobar
 *	   ^
 *     2.1 = B >> A( C(foo, bar) )  >>  2.3 = A(foobar) 
 * 
 * 3 = A(foobar)  >>  #foobar  >> "foobar"  
 * 
 * */
#define A(arg)			#arg
#define B(arg)			A(arg)
#define C(arg1, arg2)	arg1 ## arg2
#define D(arg1, arg2)   B(C(arg1, arg2))


/* __2__ : stop expanding because of meeting #
 * 
 * 1 = E(foo, bar)  >>  A( C(foo, bar) )
 * 
 * 2 = A( C(foo, bar) )  
 *     |  ^
 *     |  doesn't expanded cause on 2.1 step # has appeared
 *     ^
 *     2.1 = A  >>  #C(foo, bar)  >>  "C(foo, bar)"
 * 
 * */
#define E(arg1, arg2)   A(C(arg1, arg2))


int main(int argc, char** argv)
{
	printf(" :: __1__ : %s \n", D(foo, bar));
	printf(" :: __2__ : %s \n", E(foo, bar));
}