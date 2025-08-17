#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define ERROR(st) fprintf(stderr, st);

#define BLOCK_SIZE 4096

int main(int argc, char* argv[])
{
  if (argc != 3)
  {
    ERROR("usage: check-xor <input file> <output file>\n")
      exit(1);
  }
  unsigned char buf = 0x0;
  unsigned char chk = 0x0;
  FILE* fptr = fopen(argv[1], "rb");
  FILE* optr = fopen(argv[2], "wb");
  u_int16_t off = 0; 

  fread(&buf, sizeof(buf), 1, fptr);
  
  while(!feof(fptr))
    {
      off += 1;
      chk ^= buf;
      if (off >= BLOCK_SIZE)
	{
	  fprintf(stdout, "READ %x, CHK: %x\n", buf, chk);
	  fwrite(&chk, sizeof(chk), 1, optr);
	  off = 0;
	  chk = 0x0;
	}
      fread(&buf, sizeof(buf), 1, fptr);    
    }


  fprintf(stdout, "FINAL CHK: %x\n", chk);
  
  return 0;
}
