#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "stream.h"
#include "token.h"

#define SERV_TCP_PORT 40004
#define MAXBUF 256

#define PWD 'a'
#define DIR 'b'
#define CD 'c'
#define GET 'd'
#define PUT 'e'
#define DATA 'f'

int createTCPSocket()
{
	return socket(AF_INET, SOCKET_STREAM, 0);
}

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

void cmd_pwd(int socket)
{
	char code;
	if (sendCode(socket, PWD) == -1)
	{
		printf("Failed to send pwd command.\n");
		return;
	}

	//recv string size
	//recv data
}

void cmd_lpwd()
{
	if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", "pwd", (char*)0);
	}
}

void cmd_dir()
{

}

void cmd_ldir()
{
	if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", "dir", (char*)0);
	}
}

void cmd_cd()
{

}

void cmd_lcd()
{
	printf("\nPath: ");
	char tmp[BUFSIZE];
	scanf("%s", tmp);

	buf = "cd ";
	strcat(buf, tmp);

	if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", buf, (char*)0);
	}
}

void cmd_get(int socket, char* fn)
{
	int fd, filesize;
	char code;

	if (sendCode(socket, GET) == -1)
	{
		printf("Failed to send code.\n");
		return;
	}

	if (sendFNLen(socket, strlen(fn)) == -1)
	{
		printf("Failed to send length of filename.\n");
		return;
	}

	if (sendFN(socket, fn, strlen(fn)) == -1)
	{
		printf("Failed to send filename.\n");
		return;
	}

	if (getCode(socket, &code) == -1)
	{
		printf("Failed to send code.\n");
		return;
	}

	if (code != '1')
	{
		printf("Failed to locate file on server.\n");
		return;
	}

	if (getCode(socket, &code) == -1)
	{
		printf("Failed to get code.\n");
		return;
	}

	if (code != DATA)
	{
		printf("Failed to get a valid response from server.\n");
		return;
	}

	if (getFNLen(socket, &filesize) == -1)
	{
		printf("Failed to get file size.\n");
		return;
	}

	// Get data
	int nb = 0;
	char buf[MAXBUF];

	while ((nb = write(fn, buf, MAXBUF)) > 0)
	{
		if (getFN(socket, buf, nb) == -1)
		{
			print("Failed to send file");
			return;
		}
	}

	FILE *fp;
	fp = fopen(fn, "w");
	fprintf(fp, buf);
	fclose(fp);

	printf("%s has been received from server.\n", fn);

}

void cmd_put(int socket, char* fn)
{
	int fd, filesize;
	struct stat st; // https://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
	char code;

	// Open file
	if ((fd = open(fn, O_RDONLY)) == -1)
	{
		printf("Failed to open file: %s\n", fn);
		return;
	}

	// Get file size
	// https://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
	if (stat(fn, &st) == 0)
	{
		filesize = st.st_size;
	}
	else
	{
		printf("Failed to get file size of: %s\n", fn);
		return;
	}

	lseek(fd, 0, SEEK_SET); // Reset file to start

	// Set put code to server
	if (sendCode(socket, PUT) == -1)
	{
		printf("Failed to send put code.\n");
		return;
	}

	// Send Length of filename to server.
	if (sendFNLen(socket, strlen(fn)) == -1)
	{
		printf("Failed to send filename length.\n");
		return;
	}

	// Send filename to server.
	if (sendFN(socket, fn, strlen(fn)) <= 0)
	{
		printf("Failed to send filename.\n");
		return;
	}

	// Check for accecptance of file from server
	if (getCode(socket, &code) == -1)
	{
		print("Failed to receive code from server.\n");
		return;
	}

	if (code != PUT)
	{
		printf("Failed to get correct code from server.\n");
		return;
	}

	// Get code from server making sure they can accecpt file
	if (getCode(socket, &code) == -1)
	{
		printf("Failed to read server file creation code.\n");
		return;
	}

	// If file can't be accecpted
	if (code != '1')
	{
		printf("Server failed to accecpt put command.\n");
		return;
	}

	// Send data code
	if (sendCode(socket, DATA) == -1)
	{
		printf("Failed to send code for data transfer.\n");
		return;
	}

	// Send filesize code
	if (sendFileSize(socket, strlen(fn)) == -1)
	{
		printf("Failed to send filesize.\n");
		return;
	}

	char* mytoks[MAX_NUM_TOKENS];
	int num_of_tokens = tokenise(fn, mytoks);

	for (int i = 0; i < num_of_tokens; i++)
	{
		if (sendFN(socket, mytoks[i], MAXBUF) == -1)
		{
			printf("Failed to send data");
			return;
		}
	}

	printf("%s has been sent to server.\n", fn);
}

int main(int argc, char* argv[])
{
	int socket, n, nr, nw, i = 0;
	char buf[BUFSIZE], host[BUFSIZE];
	struct sockaddr_in ser_addr;
	struct hostent *hp;
	char* fn = "NULL";

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
		//printf("Client Input [%d]: ", i);
		printf(">> ");

		// Get user input check correct end of string char.
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

		if (pid = fork() != 0)
		{
			printf("Fork failed ... Quitting program ...\n");
			return -1;
		}

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
			cmd_pwd();
		}
		else if (strcmp(buf, "lpwd") == 0)
		{
			cmd_lpwd();
		}
		else if (strcmp(buf, "dir") == 0)
		{
			cmd_dir();
		}
		else if (strcmp(buf, "ldir") == 0)
		{
			cmd_ldir();
		}
		else if (strcmp(buf, "cd") == 0)
		{
			// get file path
			cmd_cd();
		}
		else if (strcmp(buf, "lcd") == 0)
		{
			cmd_lcd();
		}
		else if (strcmp(buf, "gets") == 0)
		{
			// get file name from user
			// check its valid
			cmd_get(socket, fn);
		}
		else if (strcmp(buf, "puts") == 0)
		{
			// get file name from user
			// check its valid
			cmd_put(socket, fn);
		}
	}
}

