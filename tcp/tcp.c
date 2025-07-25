#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
  int srv_fd, cli_fd;
  struct sockaddr_in srv_addr, cli_addr;
  char buf[BUFFER_SIZE];
  socklen_t addr_len = sizeof(cli_addr);

  srv_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (!srv_fd)
    {
      fprintf(stderr, "Error: failed to create socket\n");
      exit(1);
    }

  srv_addr.sin_family = AF_INET; // ipv4
  srv_addr.sin_addr.s_addr = INADDR_ANY; // all interfaces
  srv_addr.sin_port = htons(PORT); // big endian

  if (bind(srv_fd, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) < 0)
    {
      fprintf(stderr, "Error: failed to bind socket\n");
      close(srv_fd);
      exit(1);
    }

  if (listen(srv_fd, 1) < 0)
    {
      fprintf(stderr, "Error: failed to put socket in listen mode\n");
      close(srv_fd);
      exit(1);
    }

  printf("Server listening on port %d...\n", PORT);

  while(true)
    {
      cli_fd = accept(srv_fd, (struct sockaddr*)&cli_addr, &addr_len);

      if(cli_fd<0) {
	fprintf(stderr, "Error: failed to accept client\n");
	continue;
      }

      time_t cur_time = time(NULL);
      snprintf(buf, BUFFER_SIZE, "Current time: %s", ctime(&cur_time));
      printf("Served client %s\n", buf);

      close(cli_fd);
    }

  close(srv_fd);
  return 0;
}
