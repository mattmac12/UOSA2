#include "stream.h"

/*
int readn(int fd, char *buf, int bufsize)
{
	short data_size; 
	int n, nr, len;

	if (bufsize < BUFSIZE)
	{
		return -3; // Buffer to small
	}

	if (read(fd, (char *)&data_size, 1) != 1)
	{
		return -1;
	}

	if (read(fd, (char *)(&data_size) + 1, 1) != 1)
	{
		return -1;
	}

	len = (int)ntohs(data_size); // Convert to host byte order

	// Read len number of bytes to buf
	for (n = 0; n < len; n += nr)
	{
		if ((nr = read(fd, buf + n, len - n)) <= 0)
		{
			return (nr); // Error in reading
		}
	}

	return len;
}

int writen(int fd, char* buf, int nbytes)
{
	short data_size = nbytes;
	int n, nw;

	if (nbytes > BUFSIZE)
	{
		return -3; // Too many bytes to send in one go
	}

	// Send the data size
	data_size = htons(data_size);
	if (write(fd, (char*)&data_size, 1) != 1)
	{
		return -1;
	}

	if (write(fd, (char*)(&data_size) + 1, 1) != 1)
	{
		return -1;
	}

	// Send nbytes
	for (n = 0; n < nbytes; n += nw)
	{
		if ((nw = write(fd, buf + n, nbytes - n)) <= 0)
		{
			return nw; // Write error
		}
	}

	return n;
}
*/
// 1 byte
int sendCode(int socket, char* code)
{
	if (write(socket, (char*)&code, 1) != 1)
	{
		return -1;
	}

	return 1;
}

// 2 bytes
int sendFNLen(int socket, int len)
{
	short data = len;
	data = htons(data);

	if (write(socket, &data, 2) != 2)
	{
		return -1;
	}

	return 1;
}

// 4 bytes
int sendFileSize(int socket, int len)
{
	int data = htonl(len);
	//printf("Filesize: %d\n", len);

	if (write(socket, &data, 4) != 4)
	{
		return -1;
	}

	return 1;
}

// n bytes
int sendFN(int socket, char *buf, int nbytes)
{
	int nw = 0;
	int n = 0;
	for (n = 0; n < nbytes; n += nw)
	{
		if ((nw = write(socket, buf+n, nbytes-n)) <= 0)
			return nw;
	}

	return n;
}

int getCode(int socket, char* code) // 1 byte
{
//printf("ENTER FINE\n");
	if (read(socket, code, 1) != 1)
	{
//printf("EXIT ERROR \n");
		return -1;
	}
//printf("EXIT FINE : %c\n", &code);

	return 1;
}

int getFNLen(int socket, int* len) // 2 bytes
{
	short tmp = 0;

	if (read(socket, &tmp, 2) != 2)
	{
		return -1;
	}

	tmp = ntohs(tmp);
	
	int tmpconv = (int)tmp;

	*len = tmpconv; //here

	return 1;
}

int getFileSize(int socket, int* len) // 4 bytes
{
	int tmp = 0;
	if (read(socket, &tmp, 4) != 4)
	{
		return -1;
	}
	int tmpc = ntohl(tmp);
	*len = tmpc;
	//printf("returning\n");
	return 1;
}

int getFN(int socket, char *buf, int nbytes) // n bytes
{
	int nr = 1;
	int n = 0;
	//for (n = 0; (n < nbytes) && (nr > 0); n += nr)
	//{
		if ((nr = read(socket, buf+n, nbytes-n)) < 0)
		{
			printf("Failed reading data");
			return (nr);
		}
		//printf("%s\n", buf);
	//}
	return n;
}
