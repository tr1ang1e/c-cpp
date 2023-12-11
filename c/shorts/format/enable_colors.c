// windows only

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

int main()
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  m = ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    bool r = SetConsoleMode(h, m);
	printf("\x1B[31m %s \x1B[0m \n", "REDCOLOR");

	return 0;
}