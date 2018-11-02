#include  "stream.h"	//deals with sending and receiving to/from clients.
#include  <stdlib.h>
#include  <stdio.h>
#include  <signal.h>     /* SIGCHLD, sigaction() */
#include  <syslog.h>
#include  <string.h>     /* strlen(), strcmp() etc */
#include  <errno.h>      /* extern int errno, EINTR, perror() */
#include  <sys/types.h>  /* pid_t, u_long, u_short */
#include  <sys/socket.h> /* struct sockaddr, socket(), etc */
#include  <netinet/in.h> /* struct sockaddr_in, htons(), htonl(), */
/* and INADDR_ANY */
#include <dirent.h>
#include <fcntl.h>

#define   SERV_TCP_PORT   40004           /* server port no */
#define MAXBUF 256



void logger(int socket, char* data)
{
	FILE *fp;
	fp=fopen("logs.txt", "a");
	fprintf(fp, "[Socket: %d]: %s \n", socket, data);
	fclose(fp);

}

void claim_children()
{
     pid_t pid=1;
     
     while (pid>0) { /* claim as many zombies as we can */
         pid = waitpid(0, (int *)0, WNOHANG); 
     } 
}

void daemon_init(void)
{       
     pid_t   pid;
     struct sigaction act;

     if ( (pid = fork()) < 0) {
          perror("fork"); exit(1); 
     } else if (pid > 0) {
          printf("Hay, you'd better remember my PID: %d\n If you do, just know that it's in logger.txt ;) \n", pid);
		logger(pid, "<- this is my pid");
          exit(0);                  /* parent goes bye-bye */
     }

     /* child continues */
     setsid();                      /* become session leader */
     umask(0);                      /* clear file mode creation mask */

     /* catch SIGCHLD to remove zombies from system */
     act.sa_handler = claim_children; /* use reliable signal */
     sigemptyset(&act.sa_mask);       /* not to block other signals */
     act.sa_flags   = SA_NOCLDSTOP;   /* not catch stopped children */
     sigaction(SIGCHLD,(struct sigaction *)&act,(struct sigaction *)0);
     /* note: a less than perfect method is to use 
              signal(SIGCHLD, claim_children);
     */
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

void cmd_put(int socket)
{
	int nameLen, fileLen;
	char fileName[MAXBUF], file[MAXBUF];

	if (getFileSize(socket, &nameLen) == -1)
	{
		printf("Error getting file name size\n");
		return;
	}
	printf("File name size: %d\n", nameLen);
			
	if(getFN(socket, fileName, sizeof(fileName)) == -1)
	{
		printf("Error getting name\n");
		return;
	}
	fileName[nameLen] = '\0';
	printf("File name: %s\n", fileName);
	
	if(getFileSize(socket, &fileLen) == -1)
	{
		printf("Error getting file size\n");
		return;
	}
	printf("File size: %d\n", fileLen);

	if(getFN(socket, file, sizeof(file)) == -1)
	{
		printf("Error getting file\n");
		return;
	}
	
	FILE *fp = fopen(fileName, "w");
	fprintf(fp, file);
	fclose(fp);
	printf("File data: %s\n", file);
}

void cmd_get(int socket)
{
	int nameLen, fileLen, fd;
	char fileName[MAXBUF], file[MAXBUF];

	getFileSize(socket, &nameLen);
	printf("File name size: %d\n", nameLen);
	//daemon_init();
	getFN(socket, fileName, MAXBUF);
	fileName[nameLen] = '\0';
	printf("File name: %s", fileName);

	fileName[strlen(fileName) - 1] = '\0';

	// Open file
	if ((fd = open(fileName, O_RDONLY)) == -1)
	{
		printf("Failed to open file: %s\n", fileName);
		return;
	}
	
	// Read data
	if (read(fd, file, sizeof(file)) < 0)
	{
		printf("Read error\n");		
		return;
	}
	close (fd);

	fileLen = sizeof(file);

	printf("File size: %d\n", fileLen);
	sendFileSize(socket, fileLen);

	printf("File data: %s\n", file);
	sendFN(socket, file, fileLen);
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

		switch(code)
		{
			case 'a':
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
				cmd_get(socket);
				break;
			case 'e':
				//client putting file on server
				//receive length of filename
				//receive filename

				cmd_put(socket);
	
				break;
		}
	
	}
}

void main()
{
	int sd, nsd, n, cli_addrlen;  pid_t pid;
	struct sockaddr_in ser_addr, cli_addr;


	daemon_init();

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
