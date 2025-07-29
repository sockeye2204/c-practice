#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wordexp.h>

#define MAX_LEN 200

int main(int argc, char* argv[])
{
  int cur_arg;
  
  if (argc < 2)
    {
      fprintf(stderr, "Usage: cmd-runner <at least one arg>\n");
      exit(1);
    }

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
      }
      
      cur_arg++;
    }
}
