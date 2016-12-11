#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>

#define HOSTADDR "127.0.0.1"
//#define HOSTADDR "192.168.25.12"
#define MAXQUEUESIZE 100

void usage();
void make_socket_reusable(int listen_sfd);
void socket_bind(int listen_sfd, char *port);
void socket_listen(int listen_sfd);
int init_server(char *port);
int accept_client(int listen_sfd);

int main(int argc, char *argv[])
{
  int listen_sfd;
  int new_sfd = -1;
  struct pollfd fds[200];
  int nfds;
  int timeout;
  int status;

  if (argc < 3 || argc > 4)
  {
    usage();
    return 0;
  }
  listen_sfd = init_server(argv[1]);

  nfds = 1;
  fds[0].fd = listen_sfd;
  fds[0].events = POLLIN;
  timeout = -1;

  status = poll(fds, nfds, timeout);
  if (status == -1)
  {
    perror("poll() failed");
    //remember to close all FDs
    close(listen_sfd);
    exit(-1);
  }
  if (status == 0)
  {
    fprintf(stderr, "poll timedout\n");
    close(listen_sfd);
    exit(0);
  }

  int i;
  int current_size;

  int end_server;
  int new_size;
  new_size = nfds;
  current_size = new_size;

  for (i = 0; i < current_size; ++i)
  {
    if (fds[i].revents == 0)
      continue;

    if (!(fds[i].revents != POLLIN) && !(fds[i].revents != POLLOUT))
    {
      printf("Error! revents = %d\n", fds[i].revents);
      return 1;
    }

    if (fds[i].revents == POLLIN)
    {
      if (fds[i].fd == listen_sfd)
      {
	do
	{
	  new_sfd = accept_client(listen_sfd);
	  if (new_sfd == -2)
	  {
	    break;
	  }
	  if (new_sfd == -1)
	  {
	    end_server = 1;//change to true
	    break;
	  }
	  fds[new_size].fd = new_sfd;
	  fds[new_size].events = POLLIN;
	  ++new_size;
	  printf("Accepted %d connection\n", new_sfd);
	}while (new_sfd != -1);
      }
      else
      {
	// reading clients
      }
    }
    else if (fds[i].revents == POLLOUT)
    {
      printf("HEY YOU");
    }
  }
  //printf("A peer connected to the server\n");
  //close(new_sfd);
  //sleep(1);
  //printf("We closed the connection\n");
  close(listen_sfd);
  return 0;
}

int accept_client(int listen_sfd)
{
  int new_sfd;
  new_sfd = accept(listen_sfd, NULL, NULL);
  if (new_sfd == -1)
  {
    if (errno != EWOULDBLOCK || errno != EAGAIN)
    {
      perror("Accept() failed");
      close(listen_sfd);
      exit(-1);// instead of using exit you should end the sever
      // closing all sockets and memory
      return -1; // is new_sfd == -1 end server
    }
    return -2; // normal
  }
  return new_sfd;
}

int init_server(char *port)
{
  int listen_sfd;

  listen_sfd = socket(AF_INET, SOCK_STREAM, 0);// use | SOCK_NONBLOCK
  make_socket_reusable(listen_sfd);
  socket_bind(listen_sfd, port);
  socket_listen(listen_sfd);

  return listen_sfd;
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
