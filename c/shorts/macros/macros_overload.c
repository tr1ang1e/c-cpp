#include <stdio.h>

/*            [ INTRODUCTION ]
 *
 * At beginning we have to discover the easy way
 * to count number of arguments were passed into
 * variadic macro. Steps are:
 *
 * (0) define max number of arguments we suppose
 *     to pass to our overloaded macro. Let it be <COUNT>
 *
 * (1) define __SUPPORTING_MACRO__
 *     - <COUNT> leading args   >>  they are just plugs and not used
 *     -  RESULT argument       >>  it will keep the required result
 *     -  variadic argument     >>  for handle excess arguments
 *  #define __SUPPORTING_MACRO__(_0, _1, _2, RESULT, ...) RESULT
 *
 * (2) define __COUNT_MACRO__
 *     - only variadic argument
 *     - __SUPPORTING_MACRO__ call
 *        - __VA_ARGS__ leading
 *        - target macros instances in reverse (!) order  >>  one of them will be chosen
 *     - passing __VA_ARGS__ to the instance will be chosen by __SUPPORTING_MACRO__
 * 
 * [!] pay attention while working with MSVC compiler, due to specific  __VA_ARGS__ unpacking 
 *     approach: it considers as __VA_ARGS__ as a SINGLE argument if it makes sence in macro.
 *     See description: https://learn.microsoft.com/en-us/cpp/preprocessor ("New ... overview")
 *     To solve the problem:
 *        - use '/Zc:preprocessor' or '/experimental:preprocessor' compiler option
 *        - use workaround to expand __VA_ARGS__, e.g.:
 *              
 *              // variant 1. MSVC standard behavior
 *              #define TARGET_MACRO(arg, ...)  #arg                            // try to ignore other args
 *              #define BASE_MACRO(...) TARGET_MACRO(__VA_ARGS__)               // BASE_MACRO(1, 2) will expand to "1, 2"
 * 
 *              // variant 2. Workaround
 *              #define EXPAND(x) x
 *              #define TARGET_MACRO(arg, ...)  #arg                            // try to ignore other args
 *              #define BASE_MACRO(...) EXPAND(TARGET_MACRO(__VA_ARGS__))       // BASE_MACRO(1, 2) will expand to "1"
 * */

#define __SUPPORTING_MACRO__(_0, _1, _2, RESULT, ...) RESULT
#define __COUNT_MACRO__(...) __SUPPORTING_MACRO__(__VA_ARGS__, 3, 2, 1)

 /*            [ EXAMPLE EXPLANATION ]
  *
  * __COUNT_MACRO__('i', 'j')  >>  __SUPPORTING_MACRO__('i', 'j', 3, 2, 1)
  * __SUPPORTING_MACRO__('i', 'j', 3, 2, 1)  >>  target_2 , because:
  *      'i'  to  _0
  *      'j'  to  _1
  *       3   to  _2
  *       2   to  RESULT
  * __TARGET_MACRO__('i', 'j')  >>  RESULT  >>  2
  *
  * To improve example:
  *      - replace '3, 2, 1' in __SUPPORTING_MACRO__(__VA_ARGS__, 3, 2, 1) with required macros
  *      - add arguments passing to the end of __COUNT_MACRO__(...) definition
  *
  *
  *            [ CONSIDER THE PROBLEM ]
  *
  * The proble is that for PREPROCESSOR following macro callings:
  *      MACRO( )
  *      MACRO(1)
  * are the same  >>  and mean calling with ONE argument. So:
  *      __TARGET_MACRO__(   )  >>  __SUPPORTING_MACRO__(   , 3, 2, 1)  >>  1
  *      __TARGET_MACRO__('i')  >>  __SUPPORTING_MACRO__('i', 3, 2, 1)  >>  1
  *
  * */


  /*            [ SOLVING THE PROBLEM ]
   *
   * To solve the problem we have to make just a little fix to approach:
   *      > replace '1' in __SUPPORTING_MACRO__('i', 3, 2, 1) with smth magic macro like __1_OR_0__
   *      > use return value of __SUPPORTING_MACRO__ not for direct function call but for target function name generating
   *
   * See full solution below
   *
   * */

   // call result macro with given args
#define __OVERLOADED_MACRO__(BASE_MACRO_NAME, ...) __OVERLOADED_NAME__(BASE_MACRO_NAME, __VA_GET_COUNT__(__VA_ARGS__))(__VA_ARGS__)

// combine base macro name with given arguments count
// pay attention:
//      > need supporting macro __OVERLOADED_NAME_EXPAND__
//      > naming convention (need further): <base_name>_<args_count>
#define __OVERLOADED_NAME__(base_name, args_count)  __OVERLOADED_NAME_EXPAND__(base_name, args_count)
#define __OVERLOADED_NAME_EXPAND__(base_name, args_count)  base_name##_##args_count

// get arguments count,  typical combination
#define __VA_UP_TO_3__(_plug1, _plug2, _plug3, COUNT, ...) COUNT                                       // map given args so that we know the count
#define __VA_UP_TO_3_EXPAND__(...) __VA_UP_TO_3__(__VA_ARGS__)                                         // supporting macro
#define __VA_GET_COUNT__(...) __VA_UP_TO_3_EXPAND__(__VA_ARGS__, __AVAILABLE_COUNT__(__VA_ARGS__))     // call previous macro with given arguments  

// list available counts
// pay attention:
//      > must correspond to __VA_UP_TO_<COUNT>__ implementation 
//      > reverse order: _VA<COUNT>.._VA0)
#define __VA_0_OR_1__(...) __HAS_0_OR_1_ARGS__(__VA_ARGS__)
#define __VA_2__(...) 2         // constatnt might be used directly
#define __VA_3__(...) 3         // constatnt might be used directly
#define __AVAILABLE_COUNT__(...) __VA_3__(__VA_ARGS__), __VA_2__(__VA_ARGS__), __VA_0_OR_1__(__VA_ARGS__)

// return pattern of zeroes and ones:
//      > read description to every part 
//      > only 0001 pattern means that __VA_ARGS__ is empty (described later)
// explanation: 
//      > __HAS_COMMA__ (described later) can detect whether more than 1 operand are given
//      > __TRIGGER_PARANTHESIS__ is function-like macro (decribed later) which is helper
//      > if 0 or 1 operand is given it means that no commas are in macros call, e.g.:
//          MACRO( ) = 0 args, no commas
//          MACRO(1) = 1 arg, no commas
//          MACRO(,) = 2 args, comma is here  (attension! expression is valid)
#define __HAS_0_OR_1_ARGS__(...)                                        \
    __HAS_0_OR_1_ARGS_EXPAND__(                                         \
        __HAS_COMMA__(__VA_ARGS__),                                     /* __VA_ARGS__ with comma is in = 1    >>  0 otherwise */ \
        __HAS_COMMA__(__TRIGGER_PARENTHESIS__ __VA_ARGS__),             /* __VA_ARGS__ is () or like this = 1  >>  0 otherwise */ \
        __HAS_COMMA__(__VA_ARGS__ (__PLUG__)),                          /* __VA_ARGS__ is macro or funct. = 1  >>  0 otherwise */ \
        __HAS_COMMA__(__TRIGGER_PARENTHESIS__ __VA_ARGS__ (__PLUG__)))  /* __VA_ARGS__ is empty = 1            >>  0 otherwise */  

// function-like macro. When used inside macro call, if there are no paranthees 
// right after function-like macro it will be just skipped. Example:
//      #define MACRO(...) "smth"
//      #define FUNCTION_LIKE(...) "plug"
//      MACRO(FUNCTION_LIKE 0)  >>  MACRO(0)            // FUNCTION_LIKE just skipped
//      MACRO(FUNCTION_LIKE())  >>  MACRO("plug")  
//
// value of __TRIGGER_PARENTHESIS__(...) for current task must be ','
// If the trigger works, it will lead to call __HAS_COMMA__(,):
//      > which is absolute valid macro call
//      > which is macro call with two tokens
#define __TRIGGER_PARENTHESIS__(...) ,

// typical macro for counting args number, but result already replaced with binary value
// (remember that macro call with no args actually is the same as macro call with one arg)
//                          1      2      3            
//      __HAS_COMMA__       0      1      1
//      __ONE_OR_LESS__     1      0      0
#define __ARGS_COUNT_3__(_plug1, _plug2, _plug3, RESULT, ...) RESULT
#define __HAS_COMMA__(...)    __ARGS_COUNT_3__(__VA_ARGS__, 1, 1, 0)      // actual meaning: "more than one arguments"
#define __ONE_OR_LESS__(...)  __ARGS_COUNT_3__(__VA_ARGS__, 0, 0, 1)      // might be called "__HASNO_COMMA__" = the same

// final step of evaluating __HAS_0_OR_1_ARGS__ macro
// idea is: 
//      > create macro which will contain results of __HAS_0_OR_1_ARGS_EXPAND__ (e.g. <name>_1101 , <name>_0001 , etc.)
//      > only <name>_0001 will result in ','  <<  the only variant causes empty arguments list, other variants might be not defined
// after replacement we will have resulting macro:
//      > for <name>_0001 = __ONE_OR_LESS__(,)      = two arguments, result is 0
//      > for others = __ONE_OR_LESS__(<smth>)      =  one argument, result is 1
#define __HAS_0_OR_1_ARGS_EXPAND__(_0, _1, _2, _3) __ONE_OR_LESS__(__COMBINE_RES_TO_MACRO__(_0, _1, _2, _3))         // will lead to ...  
#define __COMBINE_RES_TO_MACRO__(_0, _1, _2, _3)  __HAS_0_OR_1_ARGS_CASE_##_0##_1##_2##_3##__
#define __HAS_0_OR_1_ARGS_CASE_0001__ ,


/*            [ USAGE ]
 *
 * #define __TARGET_MACRO__(...) __OVERLOADED_MACRO__( <BASE_NAME>, __VA_ARGS__ )
 * #define <BASE_NAME>_0( )             { ... }
 * #define <BASE_NAME>_1(_1)            { ... }
 * #define <BASE_NAME>_2(_1, _2)        { ... }
 * #define <BASE_NAME>_3(_1, _2, _3)    { ... }
 *
 * __TARGET_MACRO__();
 * __TARGET_MACRO__('i', 'j');
 *   ... etc.
 *
 * */

#define myOverloadedMacro(...) __OVERLOADED_MACRO__( myMacroFunction, __VA_ARGS__ )
#define myMacroFunction_0() "0 "
#define myMacroFunction_1( arg1 ) "1 "
#define myMacroFunction_2( arg1, arg2 ) "2 "
#define myMacroFunction_3( arg1, arg2, arg3 ) "3 "


int main()
{
    printf(myOverloadedMacro());
    printf(myOverloadedMacro(1));
    printf(myOverloadedMacro(1, 2));
    printf(myOverloadedMacro(1, 2, 3));
}
