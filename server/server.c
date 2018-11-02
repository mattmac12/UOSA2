#include  "stream.h"	//deals with sending and receiving to/from clients.
#include  <stdlib.h>
#include  <stdio.h>
#include  <string.h>     /* strlen(), strcmp() etc */
#include  <errno.h>      /* extern int errno, EINTR, perror() */
#include  <sys/types.h>  /* pid_t, u_long, u_short */
#include  <sys/socket.h> /* struct sockaddr, socket(), etc */
#include  <netinet/in.h> /* struct sockaddr_in, htons(), htonl(), */
/* and INADDR_ANY */
#include <dirent.h>

#define   SERV_TCP_PORT   40008           /* server port no */
#define MAXBUF 256

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

void cmd_pwd(int socket)
{	
	char pwd[MAXBUF];
	getcwd(pwd, MAXBUF);

	if(sendFileSize(socket, strlen(pwd)) == -1)
	{
		printf("Error sending size\n");
		return;
	}
	
	if(sendFN(socket, pwd, sizeof(pwd)) == -1)
	{
		printf("Error sending pwd string\n");
		return;
	} 
}

void cmd_dir(int socket)
{
	char dirString[MAXBUF] = "";
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			strcat(dirString, dir->d_name);
			strcat(dirString, "\n");
		}
		dirString[strlen(dirString) - 1] = '\0';
		closedir(d);
	}

	if (sendFileSize(socket, sizeof(dirString)) == -1)
	{
		printf("Error sending directory string size\n");
		return;
	}

	if (sendFN(socket, dirString, sizeof(dirString)) == -1)
	{
		printf("Error sending dirstring\n");
		return;
	}
}

void cmd_cd(int socket)
{
	char path[MAXBUF];
	int len;

	if(getFileSize(socket, &len) == -1)
	{
		printf("Error getting path size\n");
		return;
	}

	//printf("Path len: %d\n", len);

	if(getFN(socket, path, MAXBUF) == -1)
	{
		printf("Error getting path\n");
		return;
	}

	printf("Path: %s\n", path); // Spits out gibberish after path
	path[len - 1] = '\0'; // Ignores random stuff after path

	chdir(path);

}

void serve_a_client(int socket)
{
	char code;
	
	while (free) {
	
		//read data from the client
		if (getCode(socket, &code) <= 0)
		{
			exit(0); /* connection broken down */
		}
		/* read data from client */

		printf("server[%d]: Received code: %c \n", socket, code);

		int nameLen, fileLen;
		char fileName[MAXBUF], file[MAXBUF];

		switch(code)
		{
			case 'a':
				/*
				printf(""); // Does not compile unless this is here

				char tests[4];
				tests[0] = 'a';
				tests[1] = 'b';
				tests[2] = 'c';
				tests[3] = '\0';
				int test = sizeof(tests);

				sendFileSize(socket, test); //4b
				sendFN(socket, &tests, test); //nb

				printf("Completed PWD command\n");
				//pwd
				*/
				cmd_pwd(socket);
				break;
			case 'b':
				//dir
				cmd_dir(socket);
				break;
			case 'c':
				cmd_cd(socket);
				//cd
				
				break;
			case 'd':
				//client requesting file from server
				//receive length of filename
				//receive filename
				break;
			case 'e':
				//client putting file on server
				//receive length of filename
				//receive filename

				getFileSize(socket, &nameLen);
				printf("File name size: %d\n", nameLen);
			
				getFN(socket, fileName, sizeof(fileName));
				fileName[nameLen] = '\0';
				printf("File name: %s\n", fileName);

				getFileSize(socket, &fileLen);
				printf("File size: %d\n", fileLen);

				getFN(socket, file, sizeof(file));
				printf("File data: %s\n", file);
	
				break;
		}
	
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
