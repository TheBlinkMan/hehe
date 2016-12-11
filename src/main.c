#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>

void make_socket_reusable(int listen_sfd);

int main(int argc, char *argv[])
{
  int listen_sfd;
  listen_sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  make_socket_reusable(listen_sfd);
  
  return 0;
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
