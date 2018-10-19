#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "stream.h"

#define SERV_TCP_PORT 40004

int createTCPSocket()
{
	return socket(AF_INET, SOCKET_STREAM, 0);
}

int createConnection(int sd, struct sockaddr* RSA, int RSAlen)
{
	return connect(sd, RSA, RSAlen); // Backlog assumed to be 5
	// If 0 exit
}

// These will go in gets and puts below
int send(int tosend)
{
	if (sizeof(tosend) == 2)
	{
		return htons(tosend);
	}
	else
	{
		return htonl(tosend);
	}
	// followed by write() command
}

int receiver(int fromread)
{
	// read command first (toread)
	if (sizeof(int) == 2)
	{
		return ntoh(fromread);
	}
	else
	{
		return ntohl(fromread);
	}
}

// Client -> server communication requires protocol specification
// Put is done in lab 11
// address for get
void helpMenu()
{
	printf("*******************************************************************************************************");
	printf("---------------------------------------------- HELP MENU ----------------------------------------------");
	printf("pwd - Display the current directory of the server.");
	printf("lpdw - Display your current directory.");
	printf("dir - Display the file names under the current directory of the server.");
	printf("ldir - Display the file names under your current directory.");
	printf("cd <directory_pathname> - Change the current directory of the server to a specified directory.");
	printf("lcd <directory_pathname> - Change your current directory to the specified directory.");
	printf("get <filename> - Download the file with specified filename from the servers current directory.");
	printf("put <filename> - Upload the file with specified filename from your current directory to the server.");
	printf("quit - Terminate the session and close the program.");
	printf("*******************************************************************************************************");
}

void cmd_pwd()
{}

void cmd_lpwd()
{}

void cmd_dir()
{}

void cmd_ldir()
{}

void cmd_cd()
{}

void cmd_lcd()
{}

void cmd_get()
{}

void cmd_put()
{}

int main(int argc, char* argv[])
{
	int socket, n, nr, nw, i = 0;
	char buf[BUFSIZE], host[BUFSIZE];
	struct sockaddr_in ser_addr;
	struct hostent *hp;

	pid_t pid;

	// Get server host name
	if (argc == 1) // Assume server running on the local host
	{
		gethostname(host, sizeof(host));
	}
	else if (argc == 2) // Use the given host name
	{
		strcpy(host, argv[1]);
	}
	else
	{
		printf("Useage: %s [<server_host_name>]\n", argv[0]);
		exit(1);
	}

	// Get host address and build a server socket address
	bzero((char *)&ser_addr, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(SERV_TCP_PORT);

	if ((hp = gethostbyname(host)) == NULL)
	{
		printf("Host %s not found.\n", host);
		exit(1);
	}

	ser_addr.sin_addr.s_addr = *(u_long *) hp->h_addr;

	// Creat TCP socket and connect socket to server address
	socket = createTCPSocket();

	if (connect(socket, (stuct sockaddr *) &ser_addr, sizeof(ser_addr)) < 0)
	{
		perror("Client connect");
		exit(1);
	}

	while (++i)
	{
		printf("Client Input [%d]: ", i);
		fgets(buf, BUFSIZE, stdin);
		nr = strlen(buf);
		if (buf[nr - 1] == '\n'
		{
			buf[nr - 1] = '\0';
			--nr;
		}

		// From here onwards will be a if-else loop strcmp(buf, "<command>" == 0)
		// fork();
		// execl("/bin/sh", "sh", "-c", command, (char*) 0);

		pid = fork();

		if (strcmp(buf, "quit") == 0)
		{
			printf("Bye from client\n");
			exit(0);
		}
		else if (strcmp(buf, "help") == 0)
		{
			helpMenu();
		}
		else if (strcmp(buf, "pwd") == 0)
		{

		}
		else if (strcmp(buf, "lpwd") == 0)
		{
			if (pid == 0)
			{
				execl("/bin/sh", "sh", "-c", "pwd", (char*)0);
			}
		}
		else if (strcmp(buf, "dir") == 0)
		{

		}
		else if (strcmp(buf, "ldir") == 0)
		{
			if (pid == 0)
			{
				execl("/bin/sh", "sh", "-c", "dir", (char*)0);
			}
		}
		else if (strcmp(buf, "cd") == 0)
		{
			// get file path
		}
		else if (strcmp(buf, "lcd") == 0)
		{
			printf("Path: ");
			char tmp[BUFSIZE];
			scanf("%s", tmp);

			buf = "cd ";
			strcat(buf, tmp);

			if (pid == 0)
			{
				execl("/bin/sh", "sh", "-c", buf, (char*)0);
			}
		}
		else if (strcmp(buf, "gets") == 0)
		{
			// get file name
		}
		else if (strcmp(buf, "puts") == 0)
		{
			// get file name
		}
	}
}

