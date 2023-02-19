// linux only

#include <stdlib.h>
#include <stdio.h>

int main()
{
   	char* home = getenv("HOME");
	printf("%s\n", home);
}