#include <stdio.h>
#include <string.h>
#include <stdbool.h>


void remove_esc_sequence(const char* inputString, char* outputBuffer, size_t bufSize) 
{
    if (strlen(inputString) >= bufSize)
    {
        return;
    }
    
    char* escStart = "\x1B[";
    char escEnd = 'm';
    bool found = false;

    int inputLen = strlen(inputString);
    int escStartLen = strlen(escStart);
    int i, j;

    for (i = 0, j = 0; i < inputLen; i++) 
    {
        if (!found)
        {
            if (inputString[i] == escStart[0] && strncmp(&inputString[i], escStart, escStartLen) == 0) 
            {
                i += escStartLen - 1;
                found = true;
                continue;
            }

            outputBuffer[j] = inputString[i];
            ++j;
        }
        else
        {
            if (inputString[i] == escEnd)
            {
                found = false;
            }
        }
    }

    outputBuffer[j] = '\0';
}
 
 
int main() 
{
    const char* string = "Hello \x1B[38;5;227mbeautiful\x1B[0m world!";
    char buffer[64];

    remove_esc_sequence(string, buffer, sizeof(buffer));
    printf("%s\n", string);
    printf("%s\n", buffer);

    return 0;
}
