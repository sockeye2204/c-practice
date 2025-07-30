#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wordexp.h>
#include <sys/wait.h>

#define MAX_LEN 200

int main(int argc, char* argv[])
{
  int cur_arg;
  
  if (argc < 2)
    {
      fprintf(stderr, "Usage: cmd-runner <at least one arg>\n");
      exit(1);
    }

  if (argc == 3 && strcmp(argv[1], "-f") == 0)
    {
      FILE* fptr = fopen(argv[2], "r");

      char* this_str = malloc(sizeof(char)); // Gets realloc'd later anyway
      size_t this_size = 0;
      ssize_t chars_read = getline(&this_str, &this_size, fptr);
      while (chars_read != -1)
	{
	  if (chars_read > 0 && this_str[chars_read - 1] == '\n')
	    {
	      this_str[chars_read - 1] = '\0';
	    }

	  printf("\nRunning: %s\n", this_str);

	  wordexp_t p;
	  wordexp(this_str, &p, 0);

	  pid_t pid = fork();

	  if (pid < 0) {
	    fprintf(stderr, "Error: failed to fork()\n");
	    exit(2);
	  } else if (pid == 0) {
	    execvp(p.we_wordv[0], p.we_wordv);
	    wordfree(&p);
	  } else {
	    wait(NULL);
	  }
	  
	  chars_read = getline(&this_str, &this_size, fptr);	  
	}
      
    }
  else
    {
      cur_arg = 1;

      while (cur_arg < argc)
	{
	  printf("Running: %s\n", argv[cur_arg]);

	  char* this_str = argv[cur_arg];

	  wordexp_t p;
	  wordexp(this_str, &p, 0);

	  pid_t pid = fork();

	  if (pid < 0) {
	    fprintf(stderr, "Error: failed to fork()\n");
	    exit(2);
	  } else if (pid == 0) {
	    execvp(p.we_wordv[0], p.we_wordv);
	    wordfree(&p);
	  } else {
	    wait(NULL);
	  }

	  cur_arg++;
	}
  }

  return 0;
}
