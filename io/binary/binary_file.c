/* Both binary and text files might be considered as bytes sequencу
 *
 * The differences are:
 *   1) text files have escape sequnces (e.g. '\n') which might be interpreted by reader
 *   2) working with binary file means interpretation data as unsigned char sequence
 * So the binary file consumer must know how to decode data which where packed to it
 * The possible way to think of it:
 *   - text operations like a strcpy() function = always strings
 *   - binary operationc like a memcpy() function = any data types, programmer must control compatibility
 * 
 * Binary file interaction operations = 
 *   the same as text files  ::  fopen(), fclose()
 *   special read-write ops  ::  fread(), fwrite()
 *   
 * Binary file opening modes = the same as for text file + 'b'
 *   rb  ,  r+b   ::  read   , read-write
 *   wb  ,  w+b   ::  write  , read-write 
 *   ab  ,  a+b   ::  append , read-write
 *   wbx ,  w+bx  ::  create file if doesn't exist and don't share access to it while it is opened
 *   
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHARS (16)
#define STRUCTURES (3)  // should not equals 1 for correct example working (see fwrite() call)

// structure of data that would be written to and read from binary file 
typedef struct dataStruct
{
  int number;
  char buffer[CHARS];
} dataStruct;

int main(int argc, char** argv)
{
  int retCode = EXIT_FAILURE;
  int result = 0;

  // file descriptor
  FILE* file = NULL; 

  // array for data keeping
  dataStruct writeData[STRUCTURES] = { 0 };
  dataStruct readData[STRUCTURES - 1] = { 0 };    // -1 just for example of reading file not from beggining
  size_t dataSize = sizeof(dataStruct);

  do
  {
    // open binary file in write mode
    file = fopen("data", "w+b");
    if(file == NULL)
    {
      perror("fopen()");
      break;
    }

    writeData[0].number = 0;
    writeData[1].number = 1;
    writeData[2].number = 2;

    strcpy(writeData[0].buffer, "zero"  );
    strcpy(writeData[1].buffer, "first" );
    strcpy(writeData[2].buffer, "second");

    // write to binary file
    // result = number of bytes were written in number of items = 1, number of items were written otherwise
    result = fwrite(writeData, dataSize, STRUCTURES, file);
    if(result != STRUCTURES)
    {
      perror("fwrite()");    // [?] not shure errno value is set
      break;
    }

    //    try to open 'data; file here with the text editor     //
    //    it will appears like corrupted but it doesn't         //
    //    reading from it will prove it                         //

    // to ensure that all data were written to file before reading from it
    result = fflush(file);    
    if (result == EOF)
    {
      perror("fflush()");
      break;
    }  

    // set position with offset from beginnig
    result = fseek(file, dataSize, SEEK_SET); 
    if (result == -1)
    {
      perror("fseek()");
      break;
    }

    // read from file
    result = fread(&readData, dataSize, STRUCTURES - 1, file);
    if(result != STRUCTURES - 1)
    {
      perror("fread()");    // [?] not shure errno value is set
      break;
    }

    // print results
    for(int i = 0; i < STRUCTURES - 1; ++i)
    {
      printf(" :: [%d] = .number is %d, string is '%s' \n", i + 1, readData[i].number, readData[i].buffer);
    }

    retCode = EXIT_SUCCESS;
  } while (0);
  
  // close opened file
  if(file != NULL)
  {
    if(fclose(file) == EOF)
    {
      perror("fclose()");
      retCode = EXIT_FAILURE;
    }
  }

  return retCode;
}