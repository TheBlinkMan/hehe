#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
void usage();
void make_socket_reusable(int listen_sfd);
void socket_bind(int listen_sfd, char *port);
void socket_listen(int listen_sfd);

int main(int argc, char *argv[])
{
  int listen_sfd;

  if (argc < 3 || argc > 4)
  {
    usage();
    return 0;
  }
  listen_sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  make_socket_reusable(listen_sfd);
  socket_bind(listen_sfd, argv[1]);
  
  return 0;
}

/*!
 * \brief Vincula a socket a uma porta
 */
void socket_bind(int listen_sfd, char *port)
{
  struct sockaddr_in addr;
  struct sockaddr_storage length;
  int status;
  int integer_port = atoi(port);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(integer_port);
  status = inet_pton(addr.sin_family, "127.0.0.1", &addr.sin_addr);
  if (!status)
  {
    perror("inet_pton() failed, invalid host adress\n");
    exit(-1);
  }

  status = bind(listen_sfd, (struct sockaddr *)&addr, sizeof(length));
  if (status < 0)
  {
    perror("bind() failed");
    close(listen_sfd);
    exit(-1);
  }
}

/*!
 * \brief Modifica as opcoes da socket para ela ser reusavel
 */
void make_socket_reusable(int listen_sfd)
{
  int status;
  int on = 1;

  status = setsockopt(listen_sfd, SOL_SOCKET, SO_REUSEADDR,
		      (char *)&on, sizeof(on));
  if (status < 0)
  {
    perror("setsockopt() failed");
    close(listen_sfd);
    exit(-1);
  }
}

void usage()
{
  printf("\nUsage:\n\t./webserver PORT DIRECTORY\n");
}
