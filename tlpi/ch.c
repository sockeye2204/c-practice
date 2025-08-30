#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>

#define READ_BUF 0x80

int main(int argc, char* argv[])
{
  ssize_t written, readed;
  unsigned char buf[READ_BUF];
  int fd_in, fd_out;
  
  // Argument checking
  if (argc != 3) {
    fprintf(stderr, "Usage: tee <file_in> <file_out>\n");
    exit(1);
  }
  
  // Open fds
  fd_in = open(argv[1], O_RDONLY,
	    S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR |
	    S_IROTH | S_IWOTH);
  if (fd_in == -1) {
    fprintf(stderr, "Error: failed to open %s\n", argv[1]);
  }  
  
  fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,
	    S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR |
	    S_IROTH | S_IWOTH);
  if (fd_out == -1) {
    fprintf(stderr, "Error: failed to open %s\n", argv[2]);
  }

  // General idea is to read (and then write) data until
  // a 0-read occurs. We then seek for a hole. If one
  // does not exist, then we are at EOF.

  while(true)
    {
      break;
    }


  exit(EXIT_SUCCESS);
}
