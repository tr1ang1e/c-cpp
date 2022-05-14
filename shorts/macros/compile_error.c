#if defined(_WIN32)
#else
  // int array[-1];         // compile time error without message
  #error Error message
#endif

int main(int argc, char** argv)
{

  return 0;
}