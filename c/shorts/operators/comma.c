/* ',' symbol might be interpreted in two ways
 *     1 = as a separator between elements (e.g. function arguments or variable declaration)
 *     2 = as an operator with following meaning:
 *         > left part of expression would be executed first
 *         > entire expression would have type and result of write part of expression
 * 
 * */ 

#include <stdio.h>

void f(const char* message)
{
  printf("%s", message);
};

int main(int argc, char** argv)
{
  // as a separator
  int a, b, c;

  // as an operator (one argument, that's why double (( )) in call)
  f((printf("Left part of expression using comma \n"), "Wright part of the same expression \n"));

  return 0;
}