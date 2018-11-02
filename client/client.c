#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "stream.h"
#include <sys/stat.h>
#include <fcntl.h>

#define SERV_TCP_PORT 40008
#define MAXBUF 256

#define PWD 'a'
#define DIR 'b'
#define CD 'c'
#define GET 'd'
#define PUT 'e'
#define DATA 'f'

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

int createTCPSocket()
{
	return socket(PF_INET, SOCK_STREAM, 0);
}

void cmd_pwd(int socket)
{
	int dirSize;
	char buf[MAXBUF];

	// Send PWD code to server
	if (sendOneByte(socket,(char*) PWD) == -1)
	{
		printf("Failed to send pwd command.\n");
		return;
	}

	// Get size of directory
	if (getFourBytes(socket, &dirSize) == -1)
	{
		printf("Failed to get directory length.\n");
		return;
	}
	printf("dirSize: %d\n", dirSize);

	// Get directory data
	if (getNBytes(socket, buf, sizeof(buf)) == -1)
	{
		printf("Failed to get directory.\n");
		return;
	}

	printf("BUF: %s\n", buf);
}

void cmd_lpwd()
{
	pid_t pid = fork();
	
	if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", "pwd", (char*)0);
	}
}

void cmd_dir(int socket)
{
	int dirSize;
	char buf[MAXBUF];

	// Send PWD code to server
	if (sendOneByte(socket, (char*)PWD) == -1)
	{
		printf("Failed to send dir command.\n");
		return;
	}

	// Get size of directory
	if (getFourBytes(socket, &dirSize) == -1)
	{
		printf("Failed to get directory length.\n");
		return;
	}

	// Get directory data
	if (getNBytes(socket, buf, sizeof(buf)) == -1)
	{
		printf("Failed to get directory.\n");
		return;
	}

	printf("\n%s", buf);
}

void cmd_ldir()
{
	pid_t pid = fork();
		
	if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", "dir", (char*)0);
	}
}

void cmd_cd(int socket, char* cdpath)
{
	// Send cd code
	// if cd path
	// send 1 byte code for path
	// send 4 bytes for path data size
	// send n bytes for path data
	// receive ackcode
	// else
	// receive ackcode

	char* code;

	if (sendOneByte(socket, (char*)CD) == -1)
	{
		printf("Failed to send CD code.\n");
		return;
	}

	if (strcmp(cdpath, ""))
	{
		if (sendOneByte(socket, (char*) '0') == -1)
		{
			printf("Failed to send no path code.\n");
			return;
		}

		if (getOneByte(socket, code) == -1)
		{
			printf("Failed to get code back from server.\n");
			return;
		}

		printf("Directory successfully returned to home.\n");
	}
	else
	{
		if (sendOneByte(socket, (char*) '1') == -1)
		{
			printf("Failed to send path code.\n");
			return;
		}

		if (sendFourBytes(socket, sizeof(cdpath)) == -1)
		{
			printf("Failed to send size of path.\n");
			return;
		}
		
		if (sendNBytes(socket, cdpath, sizeof(cdpath)) == -1)
		{
			printf("Failed to send cdpath.\n");
			return;
		}

		if (getOneByte(socket, code) == -1)
		{
			printf("Failed to get code back from server.\n");
			return;
		}

		printf("Directory successfully changed to %s.\n", cdpath);
	}
	
}

void cmd_lcd()
{		
	printf("\nPath: ");
	char tmp[MAXBUF], buf[MAXBUF];
	scanf("%s", tmp);

	buf[0] = 'c';
	buf[1] = 'd';
	buf[2] = ' ';

	strcat(buf, tmp);

	pid_t pid = fork();
	if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", buf, (char*)0);
	}
}

void cmd_get(int socket, char* fn)
{
	// send 1 byte code - G
	// send 2 btye code in 2s compliment and in network byte order - filename len
	// send n bytes - filename

	// receive 1 byte code - 0 failed 1 good

	// receive 4 byte int - n - len of file
	// receive n bytes - file

	int fd, filesize;
	char code;

	// Send get code
	if (sendOneByte(socket, (char*) GET) == -1)
	{
		printf("Failed to send code.\n");
		return;
	}
	// Send file name length
	if (sendTwoBytes(socket, strlen(fn)) == -1)
	{
		printf("Failed to send length of filename.\n");
		return;
	}
	// Send file name
	if (sendNBytes(socket, fn, strlen(fn)) == -1)
	{
		printf("Failed to send filename.\n");
		return;
	}
	// Get confirmation code from server
	if (getOneByte(socket, &code) == -1)
	{
		printf("Failed to receive code.\n");
		return;
	}
	// Check code from server
	if (code != '1')
	{
		printf("Failed to locate file on server.\n");
		return;
	}
	// Get file length from server
	if (getTwoBytes(socket, &filesize) == -1)
	{
		printf("Failed to get file size.\n");
		return;
	}

	// Get file from server
	/* check this */
	int nb = 0;
	char buf[MAXBUF];

	while ((nb = write(socket, buf, MAXBUF)) > 0)
	{
		if (getNBytes(socket, buf, nb) == -1)
		{
			printf("Failed to send file");
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
	// 1 byte code - 'P'
	// 2 byte int 2s compliment and in network byte order len of filename to be sent by server
	// send sequence of ascii chars rep file name

	// receive ack code (0 = good)

	// 1 byte code - 'B'
	// 4 byte int in 2s compliment and in network byute order which represents the length of the file
	// n bytes - contents of file

	int fd, filesize;
	int nr = 0;
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
	if (sendOneByte(socket, (char*) PUT) == -1)
	{
		printf("Failed to send put code.\n");
		return;
	}

	// Send Length of filename to server.
	if (sendTwoBytes(socket, strlen(fn)) == -1)
	{
		printf("Failed to send filename length.\n");
		return;
	}

	// Send filename to server.
	if (sendNBytes(socket, fn, strlen(fn)) <= 0)
	{
		printf("Failed to send filename.\n");
		return;
	}

	// Check for accecptance of file from server
	/* add more error checking here */
	if (getOneByte(socket, &code) != 0)
	{
		printf("Failed to accecpt command on server.\n");
		return;
	}

	// Send data code
	if (sendOneByte(socket, (char*) DATA) == -1)
	{
		printf("Failed to send code for data transfer.\n");
		return;
	}

	// Send filesize code
	if (sendFourBytes(socket, strlen(fn)) == -1)
	{
		printf("Failed to send filesize.\n");
		return;
	}

	char* buf;
	while ((nr = read(fd, buf, MAXBUF)) > 0)
	{
		if (sendNBytes(socket, buf, nr) == -1)
		{
			printf("Failed to send file");
			return;
		}
	}

	printf("%s has been sent to server.\n", fn);
}

int main(int argc, char* argv[])
{
	int socket, n, nr, nw, i = 0;
	char buf[BUFSIZE], host[60];
	unsigned short port;
	struct sockaddr_in ser_addr;
	struct hostent *hp;
	char* fn = "NULL";

	// Get server host name
	if (argc == 1) // Assume server running on the local host
	{
		gethostname(host, sizeof(host));
		port = SERV_TCP_PORT;
	}
	else if (argc == 2) // Use the given host name
	{
		strcpy(host, argv[1]);
		port = SERV_TCP_PORT;
	}
	else if (argc == 3)
	{
		strcpy(host, argv[1]);
		int n = atoi(argv[2]);
		if (n >= 1024 && n < 65536)
		{
			port = n;
		}
		else
		{
			printf("Error: server port number must be between 1024 and 65535.\n");
			exit(1);
		}
	}
	else
	{
		printf("Useage: %s [<server_host_name>]\n", argv[0]);
		exit(1);
	}

	// Get host address and build a server socket address
	bzero((char *)&ser_addr, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);

	if ((hp = gethostbyname(host)) == NULL)
	{
		printf("Host %s not found.\n", host);
		exit(1);
	}

	//ser_addr.sin_addr.s_addr = *(unsigned long *) hp->h_addr;

	// Creat TCP socket and connect socket to server address
	socket = createTCPSocket();

	if (connect(socket, (struct sockaddr *) &ser_addr, sizeof(ser_addr)) < 0)
	{
		perror("Client connect");
		exit(1);
	}

	while (++i)
	{
		//printf("Client Input [%d]: ", i);
		printf(">> ");

		// Get user input check correct end of string char.
		fgets(buf, sizeof(buf), stdin);
		nr = strlen(buf);

		if (buf[nr - 1] == '\n')
		{
			buf[nr - 1] = '\0';
			--nr;
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
			cmd_pwd(socket);
		}
		else if (strcmp(buf, "lpwd") == 0)
		{
			cmd_lpwd();
		}
		else if (strcmp(buf, "dir") == 0)
		{
			cmd_dir(socket);
		}
		else if (strcmp(buf, "ldir") == 0)
		{
			cmd_ldir();
		}
		else if (strcmp(buf, "cd") == 0)
		{
			// get file path
			char path[MAXBUF];
			printf("\nPath: ");
			fgets(path, MAXBUF, stdin);

			cmd_cd(socket, path);
		}
		else if (strcmp(buf, "lcd") == 0)
		{
			cmd_lcd();
		}
		else if (strcmp(buf, "gets") == 0)
		{
			// get file name from user
			// check its valid
			printf("Filename: ");
			fgets(fn, MAXBUF, stdin);

			cmd_get(socket, fn);
		}
		else if (strcmp(buf, "puts") == 0)
		{
			// get file name from user
			// check its valid
			printf("Filename: ");
			fgets(fn, MAXBUF, stdin);

			cmd_put(socket, fn);
		}
	}
}

