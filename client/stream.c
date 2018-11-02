#include "stream.h"

// 1 byte
int sendOneByte(int socket, char* code)
{
	//printf("ENTER\n");
	if (write(socket, (char*)&code, 1) != 1)
	{
		//printf("ERROR\n");
		return -1;
	}
	//printf("FINE EXITING\n");

	return 1;
}

// 2 bytes
int sendTwoBytes(int socket, int len)
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
int sendFourBytes(int socket, int len)
{
	int data = htonl(len);

	if (write(socket, &data, 4) != 4)
	{
		return -1;
	}

	return 1;
}

// n bytes
int sendNBytes(int socket, char *buf, int nbytes)
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

int getOneByte(int socket, char* code) // 1 byte
{
	if (read(socket, (char*)&code, 1) != 1)
	{
		return -1;
	}

	return 1;
}

int getTwoBytes(int socket, int* len) // 2 bytes
{
	short tmp = 0;

	if (read(socket, &tmp, 2) != 2)
	{
		return -1;
	}

	tmp = ntohs(tmp);
	
	int tmpconv = (int)tmp;
	len = (int*)tmpconv; //here

	return 1;
}

int getFourBytes(int socket, int* len) // 4 bytes
{ 
//here
	int tmp = 0;
	if (read(socket, &tmp, 4) != 4)
	{
		return -1;
	}

	tmp = ntohl(tmp);
	len = &tmp;
	return 1;
}

int getNBytes(int socket, char *buf, int nbytes) // n bytes
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
