/* __1__ : compiletime assertion 
 *
 * _Static_assert = generates compiletime error
 * _Static_assert(expression, message);
 *		if expression == 1  >>  nothing happens
 *      if expression == 0  >>  compile error happens, message is printed
 * 
 * _Static_assert might be used in any place inside code
 * (of course if all necessary data is available)
 * 
 * #define static_assert _Static_assert
 * in <assert.h> header (convinient form)
 * 
 * Example: check if pudding 
 *		uncomment pragmas in following example
 *		to avoid _Static_assert error is it is
 * 
 * */

// #pragma pack(push, 1)  // uncomment to avoid _Static_assert() triggering  >>  and to deal with __2__ : runtime assertion
struct S
{
	char c;
	int i;
};
// #pragma pack(pop)
_Static_assert(sizeof(struct S) == sizeof(char) + sizeof(int),  "__1__ : struct S must not have any padding");


/* __2__ : runtime assertion
 * 
 * assert = generates runtime error
 * assert(expression && message)
 *		if expression == 1  >>  nothing happens
 *      if expression == 0  >>  compile error happens, message is printed
 * 
 * Usage:
 *		to use assert macro >> #include <assert.h>
 *      to turn all assert() off  >>  #define NDEBUG 
 *      might be used only inside functions (main(), etc.)
 * 
 * [!]
 * assert macro is implementation-defined, but common
 * idea usual is call function which ends programm execution, e.g:
 * (void)( (!!(expression) || (function(<info>), 0))
 *           ^             ^      ^              ^
 *           |             |      |              usually function() is of void type, so 0 is needed for || operator  
 *	         |             |      function which print information into stderr and stop programm execution
 *           |             right operand will be handle only if left is 0  >>  function() is called only when error
 *           cast to 0 or 1 (the result would be the same as if cast to boolean type)
 * 
 * [!]
 * asser mechanism has not facilities to pass error message directly,
 * so && inside assertion might be used: assert(0 && "zero was given");
 * the string is always 'true' and will not affect the result, but 
 * will appear in case of error
 * 
 */
// #define NDEBUG  // uncomment to turn runtime assertions off
#include <assert.h>

int main(int argc, char** argv)
{
	assert((sizeof(struct S) > sizeof(char) + sizeof(int)) && "struct S must have pudding");

	return 0;
}