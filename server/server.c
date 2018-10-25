#include  "stream.h"	//deals with sending and receiving to/from clients.
#include  <string.h>     /* strlen(), strcmp() etc */
#include  <errno.h>      /* extern int errno, EINTR, perror() */
#include  <sys/types.h>  /* pid_t, u_long, u_short */
#include  <sys/socket.h> /* struct sockaddr, socket(), etc */
#include  <netinet/in.h> /* struct sockaddr_in, htons(), htonl(), */
/* and INADDR_ANY */

#define   SERV_TCP_PORT   40004           /* server port no */
#define   BUFSIZE         (1024*256)

void daemon_init(void)
{
	pid_t   pid;

	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}
	else if (pid != 0) {
		printf("Daemon pid = %d\n", pid); // pid of your daemon
		exit(0);        /* end parent */
	}

	/* child continues */
	setsid();               /* become session leader */

	chdir("/");             /* change working directory */

	umask(0);               /* clear our file mode creation mask */
}

void serve_a_client(int sd)
{
	int nr, nw, i = 0;
	char buf[BUFSIZE];
	char code;
	
	while (++i) {
	
		//read data from the client
		if getCode(socket, &code) == -1)
		{
			exit(0); /* connection broken down */
		}
		/* read data from client */
		
		printf("server[%d]: Received code: %c \n", i, code);

		/*switch(code)
		{
			case 'a':
				
				break;
			case 'b':
				break;
			case 'c':
				break;
			case 'd':
				break;
			case 'e':
				break;
		}*/
	}
}

void main()
{
	int sd, nsd, n, cli_addrlen;  pid_t pid;
	struct sockaddr_in ser_addr, cli_addr;

	/* set up listening socket sd */
	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("server:socket"); exit(1);
	}

	/* build server Internet socket address */
	bzero((char *)&ser_addr, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(SERV_TCP_PORT);
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	/* note: accept client request sent to any one of the
	network interface(s) on this host.
	*/

	/* bind server address to socket sd */
	if (bind(sd, (struct sockaddr *) &ser_addr, sizeof(ser_addr))<0) {
		perror("server bind"); exit(1);
	}

	/* become a listening socket */
	listen(sd, 5);

	while (1) {

		/* wait to accept a client request for connection */
		cli_addrlen = sizeof(cli_addr);
		nsd = accept(sd, (struct sockaddr *) &cli_addr, &cli_addrlen);

		/* create a child process to handle this client */
		if ((pid = fork()) <0) {
			perror("fork"); exit(1);
		}
		else if (pid > 0) {
			close(nsd);
			continue; /* parent to wait for next client */
		}

		/* now in child, serve the current client */
		close(sd); serve_a_client(nsd);
	}
}