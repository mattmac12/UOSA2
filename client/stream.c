#include <sys/types.h>
#include <netinet/in.h>
#include "stream.h"

int readn(int fd, char *buf, int bufsize)
{
	short data_size; 
	int n, nr, len;

	if (bufsize < BUFSIZE)
	{
		return -3; // Buffer to0 small
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

