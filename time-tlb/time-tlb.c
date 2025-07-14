#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define MAX_PAGES 1024

int main(int argc, char *argv[])
{ 
  int jump = getpagesize() / sizeof(int);
  int *a = calloc(MAX_PAGES * jump, sizeof(int));

  for (int i = 0; i < MAX_PAGES * jump; i++)
    {
      a[i] = 0;
    }
  
  struct timespec start, end;
  
  for (int numpages = 1; numpages <= MAX_PAGES; numpages++)
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
