#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 8

int main(int argc, char* argv[])
{
  int srv_fd, new_socket, cli_sockets[MAX_CLIENTS];
  struct sockaddr_in srv_addr, cli_addr;
  char buf[BUFFER_SIZE];
  socklen_t addr_len = sizeof(cli_addr);
  fd_set read_fds;
  int max_fd;

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
      FD_ZERO(&read_fds);
      FD_SET(srv_fd, &read_fds);
      max_fd = srv_fd;

      for (int i = 0; i < MAX_CLIENTS; i++)
	{
	  int sd = cli_sockets[i];
	  if (sd > 0) FD_SET(sd, &read_fds);
	  if (sd > max_fd) max_fd = sd;
	}

      int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
      if ((activity < 0))
	{
	  fprintf(stderr, "Error: when selecting activity\n");
	  continue;
	}

      if (FD_ISSET(srv_fd, &read_fds))
	{
	  new_socket = accept(srv_fd, (struct sockaddr*)&cli_addr, &addr_len);
	  if (new_socket < 0)
	    {
	      fprintf(stderr, "Error: when accepting\n");
	      continue;
	    }

	  printf("New connection: socket fd %d, IP: %s, PORT: %d\n",
		 new_socket,
		 inet_ntoa(cli_addr.sin_addr),
		 ntohs(cli_addr.sin_port));

	  for (int i = 0; i < MAX_CLIENTS; i++)
	    {
	      if (cli_sockets[i] == 0)
		{
		  cli_sockets[i] = new_socket;
		  break;
		}
	    }
	}
      for (int i = 0; i < MAX_CLIENTS; i++)
	{
	  int sd = cli_sockets[i];

	  if (FD_ISSET(sd, &read_fds))
	    {
	      int bytes_read = read(sd, buf, BUFFER_SIZE);
	      if (bytes_read == 0)
		{
		  printf("Client disconnected: socket fd %d\n", sd);
		  close(sd);
		  cli_sockets[i] = 0;
		}
	      else
		{
		  time_t now = time(NULL);
		  snprintf(buf, BUFFER_SIZE, "Current time: %s", ctime(&now));
		  send(sd, buf, strlen(buf), 0);
		}
	    }
	}
    }

  return 0;
}
