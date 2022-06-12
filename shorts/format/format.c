#include <stdio.h>

/* See 'man console_codes' for full description
 * ECMA-48 Set Graphics Rendition
 * 
 * To change console output format use printf("<sequence>")
 *   > sequence = ESC + [ + command + m = \ + (x1B or 033) + [ + command + m
 *   > be aware of sending ESC to stdout, e.g.:
 *     - printf() doesn't recognize 0x1B as ESC (only %s to "\x1B...")
 *     - printf() doesn't recognize 27 as ECS (only %cc%s to 27, "...")
 *     - \033c (ESC + c) cleans console but \x1bc is another valid not-esc sequence (not ESC + c, just '0x1bc') 
 *     - ...
 * 
 * All of settings sections are presented below have specific 
 * command to reset defaults in addition to total reset
 * 
 * */


/* ------------------------------ DEFINES ----------------------------- */

// reset default
#define RESET   "\x1B[0m"

// background colors
#define WHITE   "\x1B[30m"

// text colors
#define BLACK   "\x1B[30m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"

// text format 
#define BOLD    "\x1B[1m"
#define HALFBR  "\x1B[2m"
#define UNDER   "\x1B[4m"


/* -------------------------- POSSIBLE USAGE -------------------------- */

#define IS_EQUAL(i, j)      \
do {                        \
                            \
  printf("%s", BOLD);       \
  printf(" :: RESULT = ");  \
  printf("%s", RESET);      \
                            \
  if (i == j)               \
  {                         \
    printf("%s", GREEN);    \
    printf("equal \n");     \
  }                         \
  else                      \
  {                         \
    printf("%s", RED);      \
    printf("not equal \n"); \
  }                         \
  printf("%s", RESET);      \
                            \
} while (0);                \


int main(int argc, char** argv)
{
  IS_EQUAL(0, 1);
  IS_EQUAL(1, 1);

  return 0;
}