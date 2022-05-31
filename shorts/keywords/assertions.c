/* _Static_assert = generates compile-time error
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

// #pragma pack(push, 1)
struct S
{
	char c;
	int i;
};
// #pragma pack(pop)

_Static_assert(sizeof(struct S) == sizeof(char) + sizeof(int),  "struct S must not have any padding");

int main(init argc, char** argv)
{

	return 0;
}