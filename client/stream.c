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
	short data = htons(len);

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
	int nw = 0;
	int n = 0;
	for (n = 0; n < nbytes; n += nw)
	{
		if ((nw = write(socket, buf+n, nbytes-n)) <= 0)
			return nw;
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

	*len = tmpconv;

	return 1;
}

int getFourBytes(int socket, int* len) // 4 bytes
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

int getNBytes(int socket, char *buf, int nbytes) // n bytes
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
