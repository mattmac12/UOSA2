#include "stream.h"

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
