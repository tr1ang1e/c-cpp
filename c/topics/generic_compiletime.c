/* Compiletime type identification =
 * choose concrete option in dependency on 'arg' type
 *
 * Might be used for:
 *   __1__ : function overloading
 *   __2__ : just substitution
 *   __3__ : type comparison
 * 
 * Attention to:
 *   > syntax (parantheses and commas)
 *   > 'default:' case (use if type was not specifed in _Generic)
 *      if no default and type was not detected = compile error
 * 
 * Users type:
 *   > might be regognized by _Generic
 *   > must be complete (incomplete types will cause compile error)
 * 
 * */ 

#include <stdio.h>
#include <stdbool.h>

// user's complete type
typedef struct S
{
	void* pointer;
} S;

// function overloading
void pfloat(float f) { printf(" ptype() macro >> pfloat() function call \n"); }
void pint(int i) { printf(" ptype() macro >> pint() function call \n"); }
#define ptype(arg) _Generic((arg),		\
				      float: pfloat,    \
					  int: pint,        \
					  default: pint     \
				   )(arg)

// just substitution
void ptypestring(const char* typestring) { printf(" %s type was detected \n", typestring); }
#define typestring(arg) _Generic((arg),		   \
	                      float: "float",      \
						  int: "int",          \
						  default: "unknown"   \
					   )

// type comparison
void psametype(bool same) { printf(" types are %s \n", same ? "same" : "different"); }
#define sametype(arg, T) _Generic((arg),       \
						    T: true,           \
							default: false     \
						 )


int main(int argc, char** argv)
{
	printf(" :: __1__ : FUNCTION OVERLOADING \n");
	float f = 0; ptype(f);
	int i = 0 ; ptype(i);

	printf(" :: __2__ : JUST SUBSTITUTION \n");
	ptypestring(typestring(1));
	ptypestring(typestring(1.0f));
	ptypestring(typestring("const char*"));

	printf(" :: __3__ : TYPE COMPARISON \n");
	S s = { 0 };
	psametype(sametype(s, S));
	psametype(sametype(i, float));

	return 0;
}