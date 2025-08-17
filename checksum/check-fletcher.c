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
  u_int16_t chk1 = 0x0;
  u_int16_t chk2 = 0x0;
  FILE* fptr = fopen(argv[1], "rb");

  fread(&buf, sizeof(buf), 1, fptr);
  
  while(!feof(fptr))
    {
      chk1 = (chk1 + buf) % 0xFF;
      chk2 = (chk2 + chk1) % 0xFF;
      fprintf(stdout, "READ %x, CHK1: %x, CHK2: %x\n", buf, chk1, chk2);
      fread(&buf, sizeof(buf), 1, fptr);    
    }


  fprintf(stdout, "FINAL CHK1: %x, CHK2: %x\n", chk1, chk2);
  
  return 0;
}
