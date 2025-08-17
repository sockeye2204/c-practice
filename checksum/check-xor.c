#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define ERROR(st) fprintf(stderr, st);

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    ERROR("usage: check-xor <file>\n")
      exit(1);
  }
  unsigned char buf = 0x0;
  unsigned char chk = 0x0;
  FILE* fptr = fopen(argv[1], "rb");

  fread(&buf, sizeof(buf), 1, fptr);
  
  while(!feof(fptr))
    {
      chk ^= buf;
      fprintf(stdout, "READ %x, CHK: %x\n", buf, chk);
      fread(&buf, sizeof(buf), 1, fptr);    
    }


  fprintf(stdout, "FINAL CHK: %x\n", chk);
  
  return 0;
}
