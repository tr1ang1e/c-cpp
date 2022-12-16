/* X-macros is not the special macros form
 * it is just the way to define such a macros,
 * which might simplify working with list of
 * different kind of declarations. So:
 *   - X-macros = self-mainaining code
 *   - X-macros = inter-dependent code
 * 
 * > X is just common used name and might be changed
 * > To understand X-macros better see the nested_macros.c
 * 
 * The example below demonstrates working enum type working.
 * Pay attention for the approach: only X-macro is declared 
 * completely >> all other cases use this macro definition
 * 
 * */

#include <stdio.h>

// define X-macro 
// from this moment every time we want to add
// new enum entry, it should be added only here
#define COLORS   \
    X(NONE)      \
	X(WHITE)     \
    X(BLACK)     \
    X(GREEN)     \
    X(YELLOW)    \
    X(PURPLE)    \
    X(MAX)       \

// preprocessor-generated enum
// after it we can use everty color, mentioned in COLORS macro
typedef enum COLOR
{
    #define X(value) value,
        COLORS
    #undef X
} COLOR;

// preprocessor-generated function
// after it we can get string representation for every color. mentioned in COLORS macro
const char* color_to_string(COLOR color)
{
    switch (color)
    {
        #define X(value)        \
            case value:         \
                return #value;
            COLORS
        #undef X
    }
}

int main(int argc, char** argv)
{
    for (COLOR color = NONE + 1; color < MAX; ++color)
    {
        printf("%s ", color_to_string(color));
    }
    printf("\n");

	return 0;
}