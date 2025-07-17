#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

// This program simulates two paging techniques: LRU (Least Recently Used) and LFU (Least Frequently Used)
// which aid in deciding which pages to page out of the cache, in the event that the cache is full and
// room needs to be made for other pages to be paged in. Importantly, this program does not deal with real
// pages, but more units of abstraction: this is not meant to be OS code, but more a demonstration of how
// these two algorithms work. The program takes one additional argument: the name of a csv stream of pages
// which will be 'accessed' in the order specified.

#define PAGE_CACHE_SIZE 5
#define MAX_READ_LENGTH 100

struct Page
{
  int vpn;
  int use:1; // Use bit
};

static struct Page sPageCache[PAGE_CACHE_SIZE] = {0};
static int sState = 0; // Program state
static int sCurPage;

int main(int argc, char* argv[])
{
  static FILE* sFilePtr;
  static char sInputPtr[MAX_READ_LENGTH];
  static char* sCurToken;
  while(1)
    {
      switch(sState)
	{
	case 0:
	  srand(time(NULL));
	  sState++;
	  break;
	case 1:
	  if (argc != 2)
	    {
	      fprintf(stderr, "Usage: paging-policy <csv page access stream>\n");
	      fflush(stderr);
	      return 1;
	    }
	  sState++;
	  break;
	case 2:
	  sFilePtr = fopen(argv[1], "r");
	  if (!sFilePtr)
	    {
	      fprintf(stderr, "Error opening file '%s' for reading.\n", argv[1]);
	      fflush(stderr);
	      return 2;
	    }
	  fgets(sInputPtr, MAX_READ_LENGTH, sFilePtr);
	  sCurToken = strtok(sInputPtr, ",");
	  sState++;
	  break;
	case 3:
	  // The main part. Basicially run strtok continuously to get the current page.
	  int foundPage;
	  int i;

	  while (sCurToken != NULL)
	    {
	      sCurPage = atoi(sCurToken);
	      sCurToken = strtok(NULL, ",");

	      foundPage = false;
	      printf("Read page %d access from stream\n", sCurPage);
	      
	      for (i = 0; i < PAGE_CACHE_SIZE; i++)
		{
		  if (sPageCache[i].vpn == sCurPage)
		    {
		      foundPage = true;
		      break;
		    }
		}

	      if (foundPage)
		{
		  printf("CACHE HIT: %d in cache slot %d\n", sCurPage, i);
		}
	      else
		{
		  printf("CACHE MISS: %d\n", sCurPage);
		}
	    }
	  sState++;
	  break;
	default:
	  fclose(sFilePtr);
	  return 0;
	}
    }
}
