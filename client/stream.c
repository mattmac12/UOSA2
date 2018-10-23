#include <sys/types.h>
#include <netinet/in.h>
#include "stream.h"

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

	if (write(socket, &data, 4) != 4)
	{
		return -1;
	}

	return 1;
}

// n bytes
int sendFN(int socket, char *buf, int nbytes)
{
	int n = 0;

	for (int i = 0; i < nbytes; i += n)
	{
		if ((n = write(socket, buf + i, nbytes - i)) <= 0)
		{
			return n; // Write error
		}
	}

	return n;
}

int getCode(int socket, char* code) // 1 byte
{
	if (read(socket, (char*)&code, 1) != 1)
	{
		return -1;
	}

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
	len = &tmpconv;

	return 1;
}

int getFileSize(int socket, int* len) // 4 bytes
{
	int tmp = 0;

	if (read(socket, &tmp, 4) != 4)
	{
		return -1;
	}

	tmp = ntohl(tmp);
	len = &tmp;
}

int getFN(int socket, char *buf, int nbytes) // n bytes
{
	int n = 0;

	for (int i = 0; i < nbytes && n > 0; i += n)
	{
		if ((n = read(socket, buf + i, nbytes - i)) <= 0)
		{
			return n; // read error
		}
	}

	return n;
}

/*
int sender(int tosend)
{
	if (sizeof(tosend) == 2)
	{
		return htons(tosend);
	}
	else
	{
		return htonl(tosend);
	}
}

int receiver(int fromread)
{
	if (sizeof(int) == 2)
	{
		return ntoh(fromread);
	}
	else
	{
		return ntohl(fromread);
	}
}
*/