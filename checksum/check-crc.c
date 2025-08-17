#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define ERROR(st) fprintf(stderr, st);

#define POLYNOMIAL 0xA001

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    ERROR("usage: check-xor <file>\n")
      exit(1);
  }
  unsigned char buf = 0x0;
  u_int16_t chk = 0xFFFF;
  FILE* fptr = fopen(argv[1], "rb");

  fread(&buf, sizeof(buf), 1, fptr);
  
  while(!feof(fptr))
    {
      chk ^= buf;
      for (int i = 0; i < 8; i++)
	{
	  if (chk & 1)
	    {
	      chk = (chk >> 1) ^ POLYNOMIAL;
	    }
	  else
	    {
	      chk >>= 1;
	    }
	}
      fprintf(stdout, "READ %x, CHK: %x\n", buf, chk);
      fread(&buf, sizeof(buf), 1, fptr);    
    }


  fprintf(stdout, "FINAL CHK: %x\n", chk);
  
  return 0;
}
