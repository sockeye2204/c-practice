#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>

#define READ_BUF 0x80

int main(int argc, char* argv[])
{
  ssize_t written, readed;
  unsigned char buf[READ_BUF];
  int fd;
  int flags = O_WRONLY | O_CREAT | O_TRUNC;
  
  // Argument checking
  if (argc < 2) {
    fprintf(stderr, "Usage: tee <output-file-name>\n");
    exit(1);
  }
  
  // Handle -a option
  if (strcmp(argv[1], "-a") == 0) {
      flags ^= O_TRUNC;
      flags |= O_APPEND;
    }

  // Open out fd
  fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC,
	    S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR |
	    S_IROTH | S_IWOTH);
  if (fd == -1) {
    fprintf(stderr, "Error: failed to open %s\n", argv[1]);
  }

  // Read from stdin and write to stdout and fd until EOF
  while((readed = read(0, /* stdin */ buf, READ_BUF)) != 0)
    {
      if (readed == -1) {
	fprintf(stderr, "Read error\n");
	exit(1);
      }

      written = write(fd, buf, readed);
      if (written == -1) {
	fprintf(stderr, "Write error\n");
	exit(1);
      }

      written = write(1, buf, readed);
      if (written == -1) {
	fprintf(stderr, "Write error\n");
	exit(1);
      }
    }

  exit(EXIT_SUCCESS);
}
