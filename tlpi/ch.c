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
  off_t old_off, new_off;
  
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
    perror("open input");
    exit(1);
  }  
  
  fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,
	    S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR |
	    S_IROTH | S_IWOTH);
  if (fd_out == -1) {
    perror("open output");
    exit(1);
  }

  // General idea is to read (and then write) data until
  // a 0-read occurs. We then seek for a hole. If one
  // does not exist, then we are at EOF.

  while(true)
    {
      // Read for data
      
      old_off = lseek(fd_in, 0, SEEK_CUR);
      printf("Current offset %x, ", old_off);
      
      readed = read(fd_in, buf, READ_BUF);
      if (readed == -1) {
	perror("read");
	exit(1);
      }

      if (readed > 0)
	{
	  written = write(fd_out, buf, readed);
	  if (written == -1) {
	    perror("write");
	    exit(1);
	  }
	  continue;
	}

      //      printf("I read %d bytes, wrote %d bytes\n", readed, written);


      // Read for holes
      
      new_off = lseek(fd_in, old_off, SEEK_HOLE);
      if (new_off == -1)
	{
	  perror("lseek SEEK_HOLE");
	  break;
	}
      
      //      printf("New offset %x\n", new_off);
      //      off_t diff = (new_off - old_off);
      //      printf("Diff %x\n", diff);

      if(lseek(fd_out, diff, SEEK_CUR) == -1) {
	perror("lseek fd_out");
	exit(1);
      }

      if(lseek(fd_in, new_off, SEEK_SET) == -1) {
	perror("lseek fd_in after hole");
	exit(1);
      }

      
    }

  close(fd_in);
  close(fd_out);

  exit(EXIT_SUCCESS);
}
