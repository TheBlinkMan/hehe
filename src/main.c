#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

//#define HOSTADDR "127.0.0.1"
#define HOSTADDR "192.168.25.12"
#define MAXQUEUESIZE 100

void usage();
void make_socket_reusable(int listen_sfd);
void socket_bind(int listen_sfd, char *port);
void socket_listen(int listen_sfd);

int main(int argc, char *argv[])
{
  int listen_sfd;
  int new_sfd = -1;

  if (argc < 3 || argc > 4)
  {
    usage();
    return 0;
  }
  listen_sfd = socket(AF_INET, SOCK_STREAM, 0);// use | SOCK_NONBLOCK
  make_socket_reusable(listen_sfd);
  socket_bind(listen_sfd, argv[1]);
  socket_listen(listen_sfd);
  new_sfd = accept(listen_sfd, NULL, NULL);
  if (new_sfd == -1)
  {
    perror("Accept() failed");
    close(listen_sfd);
    exit(-1);
  }
  printf("A peer connected to the server\n");
  close(new_sfd);
  sleep(1);
  printf("We closed the connection\n");

  close(listen_sfd);
  return 0;
}

/*!
 * \brief Marca a socket como passiva, ou seja ela recebera conexoes
 */
void socket_listen(int listen_sfd)
{
  int status;
  int backlog = MAXQUEUESIZE; //MAX QUEUE SIZE

  status = listen(listen_sfd, backlog);
  if (status == -1)
  {
    perror("listen() failed");
    close(listen_sfd);
    exit(-1);
  }
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
  status = inet_pton(addr.sin_family, HOSTADDR, &addr.sin_addr);
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
