#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
      
      int num_args = 0;

      // First get the number of args

      char* cur_str = strtok(this_str, " ");
      while(cur_str != NULL)
	{
	  printf("Argument %d: %s\n", num_args, cur_str);	  
	  num_args++;
	  cur_str = strtok(NULL, ",");
	}

      // Now sort out the args array

      char* args[num_args++];

      num_args = 0;
      cur_str = strtok(this_str, " ");
      while(cur_str != NULL)
	{
	  printf("Argument %d: %s\n", num_args, cur_str);	  
	  args[num_args] = cur_str;
	  cur_str = strtok(NULL, ",");
	}

      pid_t pid = fork();

      if (pid < 0) {
	fprintf(stderr, "Error: failed to fork()\n");
	exit(2);
      } else if (pid == 0) {
	
      } else {
      }
      
      
      
      cur_arg++;
    }
}
