#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
  int max_pages = 1024;
  
  int jump = getpagesize() / sizeof(int);

  if (argc > 1)
    {
      max_pages = atoi(argv[1]);
    }

  int *a = calloc(max_pages * jump, sizeof(int));
  
  for (int i = 0; i < max_pages * jump; i++)
    {
      a[i] = 0;
    }
  
  struct timespec start, end;
  
  for (int numpages = 1; numpages <= max_pages; numpages++)
    {
      clock_gettime(CLOCK_MONOTONIC, &start);
  for (int i = 0; i < numpages * jump; i += jump)
    {
      a[i] = a[i] + i;
      //printf("%d\n",i);
      //fflush(stdout);
    }
  clock_gettime(CLOCK_MONOTONIC, &end);
  long micros = end.tv_nsec - start.tv_nsec;
  printf("numpages = %d, time = %ldns\n", numpages, micros);
  
    }
  
  return 0;
}
